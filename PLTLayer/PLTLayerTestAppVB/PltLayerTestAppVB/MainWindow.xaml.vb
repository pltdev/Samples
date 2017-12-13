Imports System.Windows.Threading
Imports Plantronics.EZ.API

Class MainWindow
    Private plt As PLTLayer
    Private callid As Integer
    Private MY_APP_NAME As String = "PltLayerTestAppVB"

    Private Sub Window_Loaded(sender As Object, e As RoutedEventArgs)
        'Connect to Plt Layer API
        plt = PLTLayer.Instance
        AddHandler plt.PltEvent, AddressOf PltEventProc
        plt.setup(MY_APP_NAME)
    End Sub

    Private Sub Window_Closing(sender As Object, e As ComponentModel.CancelEventArgs)
        'Disconnect from Plt Layer API
        plt.shutdown()
    End Sub

    Private Sub Incoming_btn_Click(sender As Object, e As RoutedEventArgs) Handles incoming_btn.Click
        callid = callid + 1
        callid_lbl.Content = callid.ToString
        plt.on(True, callid)
    End Sub

    Private Sub outgoing_btn_Click(sender As Object, e As RoutedEventArgs) Handles outgoing_btn.Click
        callid = callid + 1
        callid_lbl.Content = callid.ToString
        plt.on(False, callid)
    End Sub

    Private Sub answer_btn_Click(sender As Object, e As RoutedEventArgs) Handles answer_btn.Click
        plt.ans(callid)
    End Sub

    Private Sub hold_btn_Click(sender As Object, e As RoutedEventArgs) Handles hold_btn.Click
        plt.hold(callid, True)
    End Sub

    Private Sub resume_btn_Click(sender As Object, e As RoutedEventArgs) Handles resume_btn.Click
        plt.hold(callid, False)
    End Sub

    Private Sub mute_btn_Click(sender As Object, e As RoutedEventArgs) Handles mute_btn.Click
        plt.mute(True)
    End Sub

    Private Sub unmute_btn_Click(sender As Object, e As RoutedEventArgs) Handles unmute_btn.Click
        plt.mute(False)
    End Sub

    Private Sub end_btn_Click(sender As Object, e As RoutedEventArgs) Handles end_btn.Click
        plt.off(callid)
    End Sub

    Private Sub PltEventProc(ByVal sender As Object, ByVal e As PltEventArgs)
        'Example processing of incoming events/parameters to inform my app
        'what Is happening with Plantronics
        Select Case e.EventType
            ' BASIC SOFTPHONE CALL CONTROL EVENTS:
            '
            Case PltEventType.CallAnswered
                AppendLog("> Plantronics answered a Softphone Call:" & vbCrLf _
                    & "Call Id: " & e.MyParams(0) & vbCrLf _
                    & "Call Source: " & e.MyParams(1))
                ' Is the call in my app?
                If e.MyParams(1) = MY_APP_NAME Then
                    AppendLog(vbCrLf & "THIS CALL ID: " & e.MyParams(0) & " IS IN MY APP!: " + MY_APP_NAME)
                    ' TODO: here you would answer the call in your app
                    Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, New Action(Sub() callid_lbl.Content = e.MyParams(0)))
                End If
            Case PltEventType.CallEnded
                AppendLog("> Plantronics ended a Softphone Call:" & vbCrLf _
                        & "Call Id: " & e.MyParams(0) & vbCrLf _
                        & "Call Source: " & e.MyParams(1))
                ' Is the call in my app?
                If e.MyParams(1) = MY_APP_NAME Then
                    AppendLog(vbCrLf & "THIS CALL ID: " & e.MyParams(0) & " IS IN MY APP!: " & MY_APP_NAME)
                    ' TODO: here you would end the call in your app
                    Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, New Action(Sub() callid_lbl.Content = e.MyParams(0)))
                End If
            Case PltEventType.Muted
                AppendLog("> Plantronics was muted")
                    ' TODO: syncronise with your app's mute feature
            Case PltEventType.UnMuted
                AppendLog("> Plantronics was un-muted")
                    ' TODO: syncronise with your app's mute feature

            ' ADVANCED SOFTPHONE CALL CONTROL EVENTS:
            '
            Case PltEventType.OnCall
                AppendLog("> Plantronics went on Softphone Call:" & vbCrLf _
                        & "Call Id: " & e.MyParams(0) & vbCrLf _
                        & "Call Source: " & e.MyParams(1) & vbCrLf _
                        & "Is Incoming?: " & e.MyParams(2) & vbCrLf _
                        & "Call State: " & e.MyParams(3))
                ' is the call in my app?
                If e.MyParams(1) = MY_APP_NAME Then
                    AppendLog(vbCrLf & "THIS CALL ID: " & e.MyParams(0) & " IS IN MY APP!: " & MY_APP_NAME)
                    ' OnCall event is for information only.
                    ' you should use CallAnswered event to answer the call in your app
                Else
                    ' TODO: optional syncronise with your app's agent availability feature
                End If

            Case PltEventType.NotOnCall
                AppendLog("> Plantronics ended a Softphone Call:" & vbCrLf _
                        & "Call Id: " & e.MyParams(0) & vbCrLf _
                        & "Call Source: " & e.MyParams(1))
                ' is the call in my app?
                If e.MyParams(1) = MY_APP_NAME Then
                    AppendLog(vbCrLf & "THIS CALL ID: " & e.MyParams(0) & "WAS IN MY APP!: " & MY_APP_NAME)
                    ' NotOnCall event is for information only.
                    ' you should use CallEnded event to end the call in your app
                Else
                    ' TODO: optional syncronise with your app's agent availability feature
                End If

            Case PltEventType.CallSwitched
                AppendLog("> Plantronics switched a Softphone Call")
                ' this event is for information only, the callid of activated call will
                ' be available via a CallAnswered event

            Case PltEventType.CallRequested
                AppendLog("> Plantronics dialpad device requested (dialed) a Softphone Call to this contact:" & vbCrLf _
                        & "Email: " & e.MyParams(0) & vbCrLf _
                        & "FriendlyName: " & e.MyParams(1) & vbCrLf _
                        & "HomePhone: " & e.MyParams(2) & vbCrLf _
                        & "Id: " & e.MyParams(3) & vbCrLf _
                        & "MobilePhone: " & e.MyParams(4) & vbCrLf _
                        & "Name: " & e.MyParams(5) & vbCrLf _
                        & "Phone: " & e.MyParams(6) & vbCrLf _
                        & "SipUri: " & e.MyParams(7) & vbCrLf _
                        & "WorkPhone: " & e.MyParams(8))
                ' TODO: optional: here you would dial an outgoing call in your app

            ' PLANTRONICS MULTI-LINE DEVICE EVENTS (e.g. Savi 700 Series):
            '
            Case PltEventType.MultiLineStateChanged
                AppendLog("> Plantronics multiline state changed: " & vbCrLf _
                        & "       PC Line: Active?: " & e.MyParams(0) & ", Held?: " & e.MyParams(1) & vbCrLf _
                        & "   Mobile Line: Active?: " & e.MyParams(2) & ", Held?: " & e.MyParams(3) & vbCrLf _
                        & "Deskphone Line: Active?: " & e.MyParams(4) & ", Held?: " & e.MyParams(5))
                ' TODO: optional syncronise with your app's agent availability feature

            ' PLANTRONICS "GENES ID" FEATURE EVENTS (DEVICE SERIAL NUMBERS):
            '
            Case PltEventType.SerialNumber
                If Not e.MyParams(0) = "" Then
                    AppendLog("> Plantronics Genes ID (Serial Number) was received:" & vbCrLf _
                                        & "Serial Number: " & e.MyParams(0) & vbCrLf _
                                          & "Serial Type: " & e.MyParams(1))

                    ' TODO: optional syncronise with your app's agent tracking system
                    ' (e.g. asset tracking or used to apply user personalised settings, i.e. on shared workstation)
                End If

            ' PLANTRONICS "CONTEXTUAL INTELLIGENCE FEATURE EVENTS:
            '
            Case PltEventType.PutOn
                AppendLog("> Plantronics was put on")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.TakenOff
                AppendLog("> Plantronics was taken off")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.Near
                AppendLog("> Plantronics in wireless range: NEAR")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.Far
                AppendLog("***************************************************************************")
                AppendLog("> Plantronics in wireless range: FAR")
                AppendLog("***************************************************************************")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.InRange
                AppendLog("> Plantronics came into wireless range")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.OutOfRange
                AppendLog("> Plantronics went out of wireless range")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.Docked
                AppendLog("> Plantronics was docked")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.UnDocked
                AppendLog("> Plantronics was un-docked")
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.Connected
                AppendLog("> Plantronics was connected to QD connector " & If(Convert.ToBoolean(e.MyParams(1)), "(initially)", ""))
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.Disconnected
                AppendLog("> Plantronics was disconnected from QD connector " & If(Convert.ToBoolean(e.MyParams(1)), "(initially)", ""))
                    ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.LineActive
                AppendLog("> Plantronics wireless link went ACTIVE.")
                    ' TODO: optional your app can know Plantronics line went active (especially for wireless products)

            Case PltEventType.LineInactive
                AppendLog("> Plantronics wireless link went in-active.")
                    ' TODO: optional your app can know Plantronics line went active (especially for wireless products)


            ' PLANTRONICS DEVICE INFORMATION EVENTS:
            '
            Case PltEventType.Attached
                Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, New Action(
                    Sub()
                        apiAttached_lbl.Content = "Yes"
                        pltDevice_lbl.Content = e.MyParams(0)
                    End Sub))
                AppendLog(vbCrLf & "> Plantronics was attached: Product Name: " _
                        & e.MyParams(0) & ", Product Id: " & Int32.Parse(e.MyParams(1)).ToString("X") & vbCrLf)
                    ' TODO: optional: switch your app to headset audio mode when Plantronics attached
            Case PltEventType.Detached
                Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, New Action(Sub() pltDevice_lbl.Content = "n/a"))
                AppendLog("> Plantronics was detached")
                    ' TODO: optional: switch your app to non-headset audio mode when Plantronics detached

            Case PltEventType.CapabilitiesChanged
                ' Optional: uncomment for debugging purposes
                '    AppendLog("  [device features: " _
                '        & "docking: " & e.MyParams(0) & ", " _
                '        & "mob callid: " & e.MyParams(1) & ", " _
                '        & "mob state: " & e.MyParams(2) & vbCrLf _
                '        & "  multiline: " & e.MyParams(3) & ", " _
                '        & "proximit: " & e.MyParams(4) & ", " _
                '        & "wearstate: " & e.MyParams(5) & ", " _
                '        & "wireless: " & e.MyParams(6) & "]")
                ' TODO: optional: use this information to know what features/events to expect from
                ' Plantronics device in your app

            ' MOBILE CALL CONTROL EVENTS (e.g. Voyager Legend, Voyager Edge, Calisto 620):
            '
            Case PltEventType.OnMobileCall
                AppendLog("> Plantronics went on Mobile Call:" & vbCrLf _
                        & "Is Incoming?: " & e.MyParams(0) & vbCrLf _
                        & "Call State: " & e.MyParams(1))
                ' TODO: optional syncronise with your app's agent availability feature

            Case PltEventType.MobileCallerId
                AppendLog("> Plantronics reported Mobile Caller Id (remote party phone number):" & vbCrLf _
                        & "Mobile Caller Id?: " & e.MyParams(0))
                ' TODO: optional syncronise with your app's contacts database / CRM system

            Case PltEventType.NotOnMobileCall
                AppendLog("> Plantronics ended a Mobile Call")
                ' TODO: optional syncronise with your app's agent availability feature


            ' OTHER DEVICE EVENTS:
            '
            Case PltEventType.BaseButtonPressed
                AppendLog("> Plantronics base button pressed: button id: " _
                        & e.MyParams(0))
                ' BaseButtonPressed event is for information only
                ' Note: some devices will generate button events internally even when no
                ' physical button is pressed.

            Case PltEventType.ButtonPressed
                AppendLog("> Plantronics button pressed: button id: " _
                        & e.MyParams(0))
                ' ButtonPressed event is for information only
                ' Note: some devices will generate button events internally even when no
                ' physical button is pressed.


            Case PltEventType.RawDataReceived
                ' for debugging:
                ' AppendLog(vbCrLf & DateTime.Now & "r:" & e.MyParams(0))
        End Select
    End Sub

    Private Sub AppendLog(ByVal msg As String)
        Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, New Action(
            Sub()
                log_textbox.AppendText(msg & vbCrLf)
                log_textbox.ScrollToEnd()
            End Sub))
    End Sub

    Private Sub clearLog_btn_Click(sender As Object, e As RoutedEventArgs) Handles clearLog_btn.Click
        log_textbox.Clear()
    End Sub
End Class
