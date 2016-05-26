
public ref class PortChat
{
private:

	static SerialPort^ _serialPort;
	static String ^serialStream;
	bool Connected;
	static array<System::String ^>^ Parser;
	static Mutex^ mut = gcnew Mutex;
	static Mutex^ mut2 = gcnew Mutex;

public:
	static bool _continue;
	PortChat()
	{
		_serialPort = gcnew SerialPort();
		Connected = 0;
	}
	array<String^>^ getSerialPorts()
	{
		array<String^>^ PortNames = gcnew array<String^>(10);
		int port_count = 0;

		//Console::WriteLine("Available Ports:");
		for each (String^ s in SerialPort::GetPortNames())
		{
			//Console::WriteLine("   {0}", s);
			PortNames[port_count] = s;
			port_count++;
		}
		PortNames[port_count] = "";

		return PortNames;
	}
	void Initialization(System::String ^port)
	{
		_serialPort->PortName = port;//cbPorts->SelectedItem->ToString();

		_serialPort->BaudRate = 9600;//PortChat::SetPortBaudRate(9600);

		_serialPort->Parity = Parity::None;

		_serialPort->DataBits = 8;//PortChat::SetPortDataBits(_serialPort->DataBits);

		_serialPort->StopBits = StopBits::One;//PortChat::SetPortStopBits(_serialPort->StopBits);

		DCB dsf; // gör en ny med normal inpu!!!

		_serialPort->DtrEnable = 0;
		_serialPort->RtsEnable = 0;

		_serialPort->Encoding->UTF8;

		//_serialPort->Handshake = Handshake::None;



		_serialPort->ReadTimeout = 1500;
		_serialPort->WriteTimeout = 1500;

		_serialPort->Open();
		Connected = true;
	}

	String ^getSerialStream()
	{
		return serialStream;
	}
	bool getSerial_Connection_Status()
	{
		return Connected;
	}

	static void Read()
	{
		while (1)
		{
			String^ message1;
			try
			{
				serialStream = _serialPort->ReadLine();

				Parser = serialStream->Split(',');
				//Console::Write(Parser[0]);
				//Console::Write(" ");
				//Console::WriteLine(Parser[1]);

			}
			catch (...)
			{
				Console::WriteLine("no data timeout...");
			}

		}
	}
	String ^getCh0()
	{
		mut->WaitOne();
		mut->ReleaseMutex();
		//Monitor::Enter(this);
		try
		{
			return Parser[0];
		}
		finally
		{
		}
		//Monitor::Exit(this);
	}
	String ^getCh1()
	{
		mut2->WaitOne();
		mut2->ReleaseMutex();
		//Monitor::Enter(this);
		try
		{
			return Parser[1];
		}
		finally
		{

		}
		//Monitor::Exit(this);
		
	}

};
