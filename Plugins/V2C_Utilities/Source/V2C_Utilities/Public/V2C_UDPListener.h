#pragma once

#include "Components/ActorComponent.h"
//#include "Sockets/Public/IPAddress.h"
#include "Common/UdpSocketBuilder.h"
#include "Common/UdpSocketReceiver.h"
#include "Common/UdpSocketSender.h"
#include "V2C_UDPListener.generated.h"


USTRUCT(BlueprintType)
struct V2C_UTILITIES_API FUDPSocketSettings
{
	GENERATED_USTRUCT_BODY()

	/** Default receiving socket IP string in form e.g. 0.0.0.0 for all connections, may need 127.0.0.1 for some cases. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	FString ip_address;

	/** Default connection port e.g. 3002*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	int32 port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	FString socket_name;

	/** in bytes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	int32 buffer_size;

	/** If true will auto-listen on begin play to port specified for receiving udp messages. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	bool open_on_begin_play;

	/** Whether we should process our data on the gamethread or the udp thread. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	bool use_game_thread;

	UPROPERTY(BlueprintReadOnly, Category = "UDP Connection Properties")
	bool port_open;
	
	FUDPSocketSettings();
};


class V2C_UTILITIES_API FUDPSocketInterface
{
public:

	TFunction<void(const FString&, const FString&, const int32&)> OnReceivedData;

	FUDPSocketSettings m_settings;


	FUDPSocketInterface();
	~FUDPSocketInterface();

	bool OpenSocket(const FString& ip_address, const int32 port);
	bool CloseSocket();

protected:
	FSocket* m_socket;
	FUdpSocketReceiver* m_socket_receiver;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUDPSocketStateSignature, int32, Port);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUDPMessageSignature, const FString&, Bytes, const FString&, IPAddress, const int32&, Port);


UCLASS(ClassGroup = "V2C_Utilities", meta = (BlueprintSpawnableComponent))
class V2C_UTILITIES_API UV2C_UDPListenerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:

	//Async events

	/** On message received on receive socket from Ip address */
	UPROPERTY(BlueprintAssignable, Category = "UDP Events")
	FUDPMessageSignature OnReceivedData;

	/** Defining UDP sending and receiving Ips, ports, and other defaults*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Connection Properties")
	FUDPSocketSettings settings;

	/** 
	* Start listening at given port for udp messages. Will auto-listen on BeginPlay by default. Listen IP of 0.0.0.0 means all connections.
	*/
	UFUNCTION(BlueprintCallable, Category = "UDP Functions")
	bool OpenSocket(const FString& ip_address = TEXT("0.0.0.0"), const int32 port = 45517);

	/**
	* Close the receiving socket. This is usually automatically done on EndPlay.
	*/
	UFUNCTION(BlueprintCallable, Category = "UDP Functions")
	bool CloseSocket();


	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	TSharedPtr<FUDPSocketInterface> m_socketinterface;
};