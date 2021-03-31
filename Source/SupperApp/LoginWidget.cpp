// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include <string>
#include "MessageDialog.h"
#include "EventDelegates.h"
#include "HomeHUD.h"

bool ULoginWidget::Initialize() {
	if (!Super::Initialize()) {
		return false;
	}

	// Login Pannel Components
	if (UEditableTextBox* txtbox = Cast<UEditableTextBox>(GetWidgetFromName("UserName_Login")))
	{
		loginAccountTb = txtbox;
	}

	if (UEditableTextBox* txtbox = Cast<UEditableTextBox>(GetWidgetFromName("password_Login")))
	{
		loginPwdTb = txtbox;
	}

	if (UEditableTextBox* txtbox = Cast<UEditableTextBox>(GetWidgetFromName("IP_Login")))
	{
		loginIpTb = txtbox;
	}

	// Register Pannel Components
	if (UEditableTextBox* txtbox = Cast<UEditableTextBox>(GetWidgetFromName("Account_Register")))
	{
		regAccountTb = txtbox;
	}

	if (UEditableTextBox* txtbox = Cast<UEditableTextBox>(GetWidgetFromName("password_Register")))
	{
		regPwdTb = txtbox;
	}

	if (UEditableTextBox* txtbox = Cast<UEditableTextBox>(GetWidgetFromName("IP_Register")))
	{
		regIpTb = txtbox;
	}

	// Local Server Detected Components
	if (UButton* btn = Cast<UButton>(GetWidgetFromName("ManualIPButton"))) 
	{
		manualIpBtn = btn;
	}

	if (UTextBlock* txtbox = Cast<UTextBlock>(GetWidgetFromName("FoundIPTextBox")))
	{
		foundIpTb = txtbox;
	}

	manualIpBtn->SetVisibility(ESlateVisibility::Hidden);
	foundIpTb->SetVisibility(ESlateVisibility::Hidden);

	UEventDelegates::OnHandleRedirectCheckMeDelegate.AddUObject(this, &ThisClass::handleRedirectToCheckMeEvent);

	if (searchLocalServer)
		SearchLocalServer();

	return true;
}

void ULoginWidget::handleSwitchLoginEvent() {
	mode = 1;
	UE_LOG(LogTemp, Warning, TEXT("mode in login"));

}

void ULoginWidget::handleSwitchRegisterEvent() {
	mode = 0;
	UE_LOG(LogTemp, Warning, TEXT("mode in register"));
}


void ULoginWidget::handleLoginClickedEvent() {

	UE_LOG(LogTemp, Warning, TEXT("login btn clicked"));
	
	FString account = loginAccountTb->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("account:%s"), *account);

	FString pwd = loginPwdTb->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("pwd:%s"), *pwd);
	FString encodedPwd = ULoginWidget::EncodeString(pwd);
	UE_LOG(LogTemp, Warning, TEXT("encoded pwd:%s"), *encodedPwd);
	FString ip = loginIpTb->GetText().ToString();
	FString port = TEXT("8086");


	TSharedPtr<FJsonObject> BaseObject = MakeShareable(new FJsonObject);
	BaseObject->SetStringField("source", "Local");

	TSharedPtr<FJsonObject> localObject = MakeShareable(new FJsonObject);
	localObject->SetStringField("user_name", *account);
	localObject->SetStringField("password_hash",*encodedPwd);

	BaseObject->SetObjectField("to_local", localObject);

	FString post_data;
	TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> jsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&post_data);
	FJsonSerializer::Serialize(BaseObject.ToSharedRef(), jsonWriter);

	//创建http 请求
	TSharedRef<IHttpRequest> requestLogin = FHttpModule::Get().CreateRequest();

	requestLogin->SetHeader("Content-Type","application/json;charset=utf-8");
	requestLogin->SetVerb("POST");
	FString URL = makeUrl(ip, port, TEXT("/temp-agent-server-api/login"));
	UE_LOG(LogTemp, Warning, TEXT("request url:%s"),*URL);
	requestLogin->SetURL(URL);
	requestLogin->SetContentAsString(post_data);

	//设置回调函数
	requestLogin->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::LoginRequestComplete);

	requestLogin->ProcessRequest();

}

void ULoginWidget::LoginRequestComplete(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess) 
{
	UE_LOG(LogTemp, Warning, TEXT("login request complete"));

	UE_LOG(LogTemp, Warning, TEXT("response code:%d"),responsePtr->GetResponseCode());
	if (!EHttpResponseCodes::IsOk(responsePtr->GetResponseCode())) {
		UE_LOG(LogTemp, Warning, TEXT("login failed!"));
		//FString err = TEXT("Login failed:") + FString::FromInt(responsePtr->GetResponseCode());
		//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(*err));
		//return;
	}

	FString responseResult = responsePtr->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("login response:%s"), *responseResult);

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(responsePtr->GetContentAsString());

	TSharedPtr<FJsonValue> rRoot;
	bool success = FJsonSerializer::Deserialize(jsonReader, rRoot);
	if (success) {
		TSharedPtr<FJsonObject> rObject = rRoot->AsObject();
		int code = rObject->GetIntegerField(TEXT("err_code"));
		
		if (0 == code) 
		{
			//FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(TEXT("Login successful!")));
			FString ip = loginIpTb->GetText().ToString();
			FString port = TEXT("8086");
			FString url = makeUrl(ip, port, TEXT("/mobile"));
			UE_LOG(LogTemp, Warning, TEXT("mobile url:%s"), *url);
			UEventDelegates::OnLoginResultDelegate.Broadcast(url);

			OnExitingLoginWidget();
		}
		else {
			FString msg = rObject->GetStringField(TEXT("err_detail"));
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
		}
	}

	   
}

void ULoginWidget::handleRegisterClickedEvent() 
{
	FString post_data;
	TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> jsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&post_data);


	FString account = regAccountTb->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("account:%s"), *account);

	FString pwd = regPwdTb->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("pwd:%s"), *pwd);
	//FString encodedPwd = ULoginWidget::HashStringWithSHA1(pwd);

	FString ip = regIpTb->GetText().ToString();
	FString port = TEXT("8086");

	//post 数据
	jsonWriter->WriteObjectStart();
	/// 写入数据
	jsonWriter->WriteValue("account", *account);
	jsonWriter->WriteValue("pwd", *pwd);
	jsonWriter->WriteObjectEnd();
	jsonWriter->Close();

	//创建http 请求
	TSharedRef<IHttpRequest> requestLogin = FHttpModule::Get().CreateRequest();

	requestLogin->SetHeader("Content-Type", "application/json;charset=utf-8");
	requestLogin->SetVerb("POST");

	FString url = makeUrl(ip, port, TEXT("/rdp/V1/developer/signup"));

	requestLogin->SetURL(url);
	requestLogin->SetContentAsString(post_data);

	//设置回调函数
	requestLogin->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::RegisterRequestComplete);

	requestLogin->ProcessRequest();

}

void ULoginWidget::RegisterRequestComplete(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess) 
{
	UE_LOG(LogTemp, Warning, TEXT("register request complete"));

	UE_LOG(LogTemp, Warning, TEXT("response code:%d"), responsePtr->GetResponseCode());
	if (!EHttpResponseCodes::IsOk(responsePtr->GetResponseCode())) {
		UE_LOG(LogTemp, Warning, TEXT("register failed!"));
	}

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(responsePtr->GetContentAsString());


	FString responseResult = responsePtr->GetContentAsString();


	UE_LOG(LogTemp, Warning, TEXT("register response:%s"), *responseResult);
}

void ULoginWidget::Login2Rdp() {

	FString post_data;
	TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> jsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&post_data);


	FString account = loginAccountTb->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("account:%s"), *account);

	FString pwd = loginPwdTb->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("pwd:%s"), *pwd);
	FString encodedPwd = ULoginWidget::HashStringWithSHA1(pwd);

	FString ip = loginIpTb->GetText().ToString();
	FString port = TEXT("8086");

	//post 数据
	jsonWriter->WriteObjectStart();
	/// 写入数据
	jsonWriter->WriteValue("account", *account);
	jsonWriter->WriteValue("pwd", *encodedPwd);
	jsonWriter->WriteObjectEnd();
	jsonWriter->Close();

	//创建http 请求
	TSharedRef<IHttpRequest> requestLogin = FHttpModule::Get().CreateRequest();

	requestLogin->SetHeader("Content-Type", "application/json;charset=utf-8");
	requestLogin->SetVerb("POST");
	FString url = makeUrl(ip, port, TEXT("/rdp/V1/developer/signin"));

	UE_LOG(LogTemp, Warning, TEXT("request url:%s"), *url);
	requestLogin->SetURL(url);
	requestLogin->SetContentAsString(post_data);

	//设置回调函数
	requestLogin->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::Login2RdpRequestComplete);

	requestLogin->ProcessRequest();
}

void ULoginWidget::Login2RdpRequestComplete(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("login request complete"));

	UE_LOG(LogTemp, Warning, TEXT("response code:%d"), responsePtr->GetResponseCode());
	if (!EHttpResponseCodes::IsOk(responsePtr->GetResponseCode())) {
		UE_LOG(LogTemp, Warning, TEXT("login failed!"));

		return;
	}

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(responsePtr->GetContentAsString());

	TSharedPtr<FJsonValue> rRoot;
	bool success = FJsonSerializer::Deserialize(jsonReader, rRoot);
	if (success) {
		TSharedPtr<FJsonObject> rObject = rRoot->AsObject();
		int code = rObject->GetIntegerField(TEXT("code"));
		FString msg = rObject->GetStringField(TEXT("messages"));
		if (0 == code)
		{
			//FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(msg));
		}
		else {
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
		}
	}
	FString responseResult = responsePtr->GetContentAsString();


	UE_LOG(LogTemp, Warning, TEXT("login response:%s"), *responseResult);

}

void ULoginWidget::handleRedirectToCheckMeEvent() {
	UE_LOG(LogTemp, Warning, TEXT("handleRedirectToCheckMeEvent Called"));
	UEventDelegates::OnRedirectCheckMeDelegate.Broadcast();
	OnExitingLoginWidget();

}

FString ULoginWidget::EncodeString(const FString &InString)
{
	FString salt = TEXT("Work hard, study hard, play hard, enjoy the life");
	FSHAHash StringHash;
	//FSHA1::HashBuffer(TCHAR_TO_ANSI(*content), content.Len(), StringHash.Hash);
	FSHA1 Sha;
	Sha.Update((uint8 *)TCHAR_TO_ANSI(*salt), salt.Len());
	Sha.Update((uint8 *)TCHAR_TO_ANSI(*InString), InString.Len());
	Sha.Update((uint8 *)TCHAR_TO_ANSI(*salt), salt.Len());
	Sha.Final();
	Sha.GetHash(StringHash.Hash);

	return StringHash.ToString().ToLower();
}

FString ULoginWidget::HashStringWithSHA1(const FString &inString)
{
	FSHAHash StringHash;
	FString salt = TEXT("aFjq3ip9n");
	FSHA1 Sha;

	Sha.Update((uint8 *)TCHAR_TO_ANSI(*salt),salt.Len());

	Sha.Update((uint8 *)TCHAR_TO_ANSI(*inString),inString.Len());

	for (int i = 1; i <1024; i++) {
		Sha.Final();
		uint8 Result[20];
		Sha.GetHash(Result);
		

		//UE_LOG(LogTemp, Warning, TEXT("result:%s"),*result);

		Sha.Reset();
		Sha.Update(Result,20);
	}

	Sha.Final();
	Sha.GetHash(StringHash.Hash);
	FString result = StringHash.ToString().ToLower();
	UE_LOG(LogTemp, Warning, TEXT("result:%s"), *result);


	return  result;
}

FString ULoginWidget::makeUrl(const FString& ip, const FString& port, const FString& route) {
	return TEXT("http://") + ip + TEXT(":") + port + route;
}

void ULoginWidget::SearchLocalServer() {

	FString ip = TEXT("192.168.0.");
	FString port = TEXT("8086");
	for (int i = 1; i < 20; i++) {

		FString local_addr = makeUrl(ip + FString::FromInt(i), port, TEXT(""));
		TSharedRef<IHttpRequest> pingRequest = FHttpModule::Get().CreateRequest();
		pingRequest->SetVerb("GET");
		// pingRequest->SetHeader("Content-Type", "application/json");
		pingRequest->SetURL(local_addr);
		pingRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnPingCompleted);
		pingRequest->ProcessRequest();
	}

}

void ULoginWidget::OnPingCompleted(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccessful) {
	
	// if (bIsSuccessful && responsePtr->GetContentType() == "application/json") {
	if (bIsSuccessful) {
		//chop off the 'http://' and ':<port>' portion
		FString local_addr = requestPtr->GetURL().RightChop(7).LeftChop(5);  
		UE_LOG(LogTemp, Warning, TEXT("local server found, ip: %s"), *local_addr);
		// set loginIP
		loginIpTb->SetText(FText::FromString(*local_addr));
		loginIpTb->SetVisibility(ESlateVisibility::Hidden);
		// set Button and Textbox
		manualIpBtn->SetVisibility(ESlateVisibility::Visible);
		foundIpTb->SetVisibility(ESlateVisibility::Visible);
		foundIpTb->SetText(FText::FromString(FString("Server Detected:") + local_addr));
		//foundIpTb->SetText(FText::FromString( FString("Server Detected:")));

	}
	/*else {
		UE_LOG(LogTemp, Warning, TEXT("local server NOT found"));
	}*/
}