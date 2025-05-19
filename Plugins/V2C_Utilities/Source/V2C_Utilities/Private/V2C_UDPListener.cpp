#include "V2C_UDPListener.h"
#include "Async/Async.h"
#include "SocketSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"



UV2C_UDPListenerComponent::UV2C_UDPListenerComponent(const FObjectInitializer &init) : UActorComponent(init)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;

	m_socketinterface = MakeShareable(new FUDPSocketInterface);

	//add event callback
	m_socketinterface->OnReceivedData = [this](const FString& Data, const FString& Endpoint, const int32& Port)
	{
		OnReceivedData.Broadcast(Data, Endpoint, Port);
	};
}


bool UV2C_UDPListenerComponent::CloseSocket()
{
	return m_socketinterface->CloseSocket();
}


bool UV2C_UDPListenerComponent::OpenSocket(const FString& ip_address, const int32 port)
{
	m_socketinterface->m_settings.open_on_begin_play = settings.open_on_begin_play;
	m_socketinterface->m_settings.socket_name = settings.socket_name;
	m_socketinterface->m_settings.buffer_size = settings.buffer_size;

	return m_socketinterface->OpenSocket(ip_address, port);
}

void UV2C_UDPListenerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UV2C_UDPListenerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UV2C_UDPListenerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	m_socketinterface->m_settings = settings;

	if (settings.open_on_begin_play)
	{
		OpenSocket(settings.ip_address, settings.port);
	}
}

void UV2C_UDPListenerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseSocket();

	m_socketinterface->OnReceivedData = nullptr;

	Super::EndPlay(EndPlayReason);
}




FUDPSocketInterface::FUDPSocketInterface()
{
	m_socket = nullptr;

	OnReceivedData = nullptr;
}



FUDPSocketInterface::~FUDPSocketInterface()
{
	if (m_settings.port_open)
	{
		CloseSocket();
		OnReceivedData = nullptr;
	}
}


bool FUDPSocketInterface::OpenSocket(const FString& ip_address, const int32 port)
{
	m_settings.ip_address = ip_address;
	m_settings.port = port;


	bool l_success = true;

	if (m_settings.port_open)
	{
		l_success = CloseSocket();
	}

	FIPv4Address l_addr;
	FIPv4Address::Parse(m_settings.ip_address, l_addr);

	//Create Socket
	FIPv4Endpoint l_endpoint(l_addr, m_settings.port);

	m_socket = FUdpSocketBuilder(*m_settings.socket_name)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(l_endpoint)
		.WithReceiveBufferSize(m_settings.buffer_size);

	FTimespan l_thread_wait_time = FTimespan::FromMilliseconds(100);
	FString l_thread_name = FString::Printf(TEXT("FUDPSocketInterface"));
	m_socket_receiver = new FUdpSocketReceiver(m_socket, l_thread_wait_time, *l_thread_name);

	m_socket_receiver->OnDataReceived().BindLambda([this](const FArrayReaderPtr& DataPtr, const FIPv4Endpoint& Endpoint)
	{
		if (!OnReceivedData)
		{
			return;
		}

		TArray<uint8> Data;
		Data.AddUninitialized(DataPtr->TotalSize());
		DataPtr->Serialize(Data.GetData(), DataPtr->TotalSize());
		
		FString contents;
		FFileHelper::BufferToString(contents, Data.GetData(), Data.Num());

		FString SenderIp = Endpoint.Address.ToString();
		int32 SenderPort = Endpoint.Port;

		if (m_settings.use_game_thread)
		{
			//Copy data to receiving thread via lambda capture
			AsyncTask(ENamedThreads::GameThread, [this, contents, SenderIp, SenderPort]()
			{
				//double check we're still bound on this thread
				if (OnReceivedData)
				{
					OnReceivedData(contents, SenderIp, SenderPort);
				}
			});
		}
		else
		{
			OnReceivedData(contents, SenderIp, SenderPort);
		}
	});

	m_settings.port_open = true;

	m_socket_receiver->Start();

	return l_success;
}

bool FUDPSocketInterface::CloseSocket()
{
	bool l_success = true;
	m_settings.port_open = false;

	if (m_socket)
	{
		m_socket_receiver->Stop();
		delete m_socket_receiver;
		m_socket_receiver = nullptr;

		l_success = m_socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(m_socket);
		m_socket = nullptr;
	}

	return l_success;
}


FUDPSocketSettings::FUDPSocketSettings()
{
	ip_address = FString(TEXT("0.0.0.0"));
	port = 45517;
	socket_name = FString(TEXT("ue5-udplistener"));

	port_open = false;

	buffer_size = 512;
	
	open_on_begin_play = true;
	use_game_thread = true;
}