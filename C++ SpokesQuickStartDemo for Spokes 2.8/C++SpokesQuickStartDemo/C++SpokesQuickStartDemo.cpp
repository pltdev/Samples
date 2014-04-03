// C++CallControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SpokesWrapper.h"

class MySpokesIntegration : public ISpokesEvents
{
private:
	Spokes * m_spokes;
public:
	MySpokesIntegration(void)
	{
		m_spokes = Spokes::GetInstance();
		m_spokes->SetEventHandler(this);
	}

	~MySpokesIntegration(void)
	{
		delete m_spokes;
		m_spokes = NULL;
	}

	virtual void MySpokesIntegration::Spokes_TakenOff(EventArgs * e)
	{
		cout << "Headset is not worn" << endl;
		delete e;
	}

	virtual void MySpokesIntegration::Spokes_PutOn(EventArgs * e)
	{
		cout << "Headset is worn" << endl;
		delete e;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	MySpokesIntegration * mySpokesIntegration = new MySpokesIntegration();
	Spokes::GetInstance()->Connect("Spokes Quick Start"); // do this after instantiation of MySpokesIntegration (i.e. not in constructor)
												  // so that C++ can sort out its virtual method tables else we won't receive Spokes events

	cout << "Press enter to quit..." << endl;
	cin.ignore();

	Spokes::GetInstance()->Disconnect();
	delete mySpokesIntegration;

	return 0;
}

