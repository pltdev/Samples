/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package MyJavaApplication;

import com.plantronics.UC.JavaSocketWrapper.SpokesConnector;
import java.io.Console;

/**
 *
 * @author lcollins
 */
public class MyJavaApplication {

    static MySpokesEventHandler mySpokesEventHandler;
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        
        System.out.println("Starting My Java Application...");
        
        SpokesConnector spokesconnector = new SpokesConnector();
        mySpokesEventHandler = new MySpokesEventHandler();
        spokesconnector.SetSpokesEventHandler(mySpokesEventHandler);
        spokesconnector.ConnectToSpokes();
        
        int num = 0;
        System.out.println("Entering loop to read user commands...");        
        boolean quit = false;
        Console c = System.console();
        if (c == null)
        {
            System.err.println("No console. Will now exit.");
            quit = true;
            spokesconnector.quit = true;
        }
        // temp variables used for parameters...
        String command, callid, contact;
        String activate, hold;
        while (!quit)
        {
            command = c.readLine("Enter a command (or type help): ");
            if (command.length()>0)
            {
                switch (command.toLowerCase())
                {
                    case "ansmob": 
                        spokesconnector.sendCommand(command);
                        break;
                    case "audioon": 
                        spokesconnector.sendCommand(command+",True");
                        break;
                    case "audiooff": 
                        spokesconnector.sendCommand(command+",False");
                        break;
                    case "dialmob": 
                        callid = c.readLine("Enter a number to dial: ");
                        spokesconnector.sendCommand(command+","+callid);
                        break;
                    case "endcall": 
                        callid = c.readLine("Enter a call id: ");
                        spokesconnector.sendCommand(command+","+callid);
                        break;
                    case "endmobcall": 
                        spokesconnector.sendCommand(command);
                        break;
                    case "incoming": 
                        callid = c.readLine("Enter a call id: ");
                        contact = c.readLine("Enter a contact name: ");
                        spokesconnector.sendCommand(command+","+callid+","+contact);
                        break;
                    case "muteon": 
                        spokesconnector.sendCommand(command+",True");
                        break;
                    case "muteoff": 
                        spokesconnector.sendCommand(command+",False");
                        break;
                    case "outgoing": 
                        callid = c.readLine("Enter a call id: ");
                        contact = c.readLine("Enter a contact name: ");
                        spokesconnector.sendCommand(command+","+callid+","+contact);
                        break;
                    case "reqserial": 
                        callid = c.readLine("Enter a serial type number (1 = base/dongle, 2 = headset): ");
                        spokesconnector.sendCommand(command+","+callid);
                        break;
                    case "setlineactive": 
                        callid = c.readLine("Enter a line type number (1 = PC, 2 = Mobile, 3 = Desk): ");
                        activate = c.readLine("Activate? (1 = Yes, 2 = No): ");
                        spokesconnector.sendCommand(command+","+callid+","+activate);
                        break;
                    case "setlinehold": 
                        callid = c.readLine("Enter a line type number (1 = PC, 2 = Mobile, 3 = Desk): ");
                        hold = c.readLine("Hold? (1 = Yes, 2 = No): ");
                        spokesconnector.sendCommand(command+","+callid+","+hold);
                        break;
                    case "quit": 
                        spokesconnector.sendCommand(command); // use server to ping my read thread to quit
                        quit = true;
                        spokesconnector.quit = true;
                        //System.out.println("Threads: ");
                        //Thread.currentThread().getThreadGroup().list();
                        break;
                    case "help":
                        ShowSupportedCommands(); 
                        break;
                    default:
                        System.out.println("WARNING: Command not recognised.");
                        ShowSupportedCommands();
                        break;
                }
            }
        }
    }

    private static void ShowSupportedCommands() 
    {
        System.out.println("Supported commands (you will be prompted to enter any parameters)");
        System.out.println("ansmob:        calls AnswerMobileCall");
        System.out.println("audioon:       calls ConnectAudioLinkToDevice( true )");
        System.out.println("audiooff:      calls ConnectAudioLinkToDevice( true )");
        System.out.println("dialmob:       calls DialMobileCall ( number )");
        System.out.println("endcall:       calls EndCall ( CallId )");
        System.out.println("endmobcall:    calls EndMobileCall");
        System.out.println("incoming:      calls IncomingCall( CallId, ContactName )");
        System.out.println("muteon:        calls SetMute ( true )");
        System.out.println("muteoff:       calls SetMute ( false )");
        System.out.println("outgoing:      calls OutgoingCall( CallId, ContactName )");
        System.out.println("reqserial:     calls RequestSingleSerialNumber( SerialType )");
        System.out.println("setlineactive: calls SetLineActive( LineType )");
        System.out.println("setlinehold:   calls SetLineHold( LineType )");       
        System.out.println("quit: close application");
    }
}
