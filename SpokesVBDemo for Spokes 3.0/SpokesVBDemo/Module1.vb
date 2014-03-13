Imports Plantronics.UC.SpokesWrapper
Imports System.Threading

Module Module1
    'Make a friend of Spokes object so we can receive it's events
    Friend WithEvents MySpokes As Spokes

    <MTAThread()>
    Sub Main()
        'Grab the Spokes make instance
        MySpokes = Spokes.Instance

        'Connect to Spokes (register our Spokes session)
        MySpokes.Connect("Spokes VB.NET Quick Start for Spoke 3.0")

        'Note: putting Console input (ReadLine) on seperate thread
        'to avoid COM getting blocked!
        Dim GUIThread As New Thread(AddressOf GUIMain)
        GUIThread.Start() ' Start the new thread.
        GUIThread.Join()

        'Disconnect from Spokes
        MySpokes.Disconnect()
    End Sub

    Sub GUIMain()
        Console.WriteLine("--")
        Console.WriteLine("Welcome to Spokes VB.NET Quick Start for Spokes 3.0!")
        Console.WriteLine("Press enter to quit...")
        Console.WriteLine("--")
        Console.ReadLine()
    End Sub

    Public Sub MySpokes_Attached(ByVal sender As Object, _
    ByVal e As Plantronics.UC.SpokesWrapper.AttachedArgs) Handles MySpokes.Attached
        Console.WriteLine("Headset attached: " + e.m_device.ProductName)
    End Sub

    Public Sub MySpokes_Detached(ByVal sender As Object, _
        ByVal e As EventArgs) Handles MySpokes.Detached
        Console.WriteLine("Headset detached")
    End Sub

    Public Sub MySpokes_PutOn(ByVal sender As Object, _
        ByVal e As Plantronics.UC.SpokesWrapper.WearingStateArgs) Handles MySpokes.PutOn
        Console.WriteLine("Headset is worn")
    End Sub

    Public Sub MySpokes_TakenOff(ByVal sender As Object, _
        ByVal e As Plantronics.UC.SpokesWrapper.WearingStateArgs) Handles MySpokes.TakenOff
        Console.WriteLine("Headset is not worn")
    End Sub

    Public Sub MySpokes_Docked(ByVal sender As Object, _
        ByVal e As Plantronics.UC.SpokesWrapper.DockedStateArgs) Handles MySpokes.Docked
        Console.WriteLine("Headset is docked")
    End Sub

    Public Sub MySpokes_UnDocked(ByVal sender As Object, _
        ByVal e As Plantronics.UC.SpokesWrapper.DockedStateArgs) Handles MySpokes.UnDocked
        Console.WriteLine("Headset is not docked")
    End Sub
End Module
