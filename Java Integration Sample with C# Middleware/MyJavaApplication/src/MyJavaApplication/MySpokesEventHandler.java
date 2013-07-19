/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package MyJavaApplication;

import com.plantronics.UC.JavaSocketWrapper.*;

/**
 *
 * @author lcollins
 */
public class MySpokesEventHandler implements ISpokesEvents 
{      
    public void Spokes_TakenOff(WearingStateArgs e)
    {
        System.out.println("Spokes_TakenOff: worn = " + e.m_worn + ", is initial state event = " + e.m_isInitialStateEvent);
    }
    
    public void Spokes_PutOn(WearingStateArgs e)
    {
        System.out.println("Spokes_PutOn: worn = " + e.m_worn + ", is initial state event = " + e.m_isInitialStateEvent);
    }
    
    public void Spokes_Near(EventArgs e)
    {
        System.out.println("Spokes_Near");
    }
    
    public void Spokes_Far(EventArgs e)
    {
        System.out.println("Spokes_Far");
    }
    
    public void Spokes_InRange(EventArgs e)
    {
        System.out.println("Spokes_InRange");
    }
    
    public void Spokes_OutOfRange(EventArgs e)
    {
        System.out.println("Spokes_OutOfRange");
    }
    
    public void Spokes_Docked(DockedStateArgs e)
    {
        System.out.println("Spokes_Docked");
    }
    
    public void Spokes_UnDocked(DockedStateArgs e)
    {
        System.out.println("Spokes_UnDocked");
    }
    
    public void Spokes_MobileCallerId(MobileCallerIdArgs e)
    {
        System.out.println("Spokes_MobileCallerId: callerid = "+e.MobileCallerId);
    }
    
    public void Spokes_OnMobileCall(OnMobileCallArgs e)
    {
        System.out.println("Spokes_OnMobileCall: incoming? = "+e.Incoming+", state = "+e.State);
    }
    
    public void Spokes_NotOnMobileCall(EventArgs e)
    {
        System.out.println("Spokes_NotOnMobileCall");
    }
    
    public void Spokes_SerialNumber(SerialNumberArgs e)
    {
        System.out.println("Spokes_SerialNumber: serialnum = "+e.SerialNumber+", serialtype = "+e.SerialNumberType);
    }
    
    public void Spokes_CallAnswered(CallAnsweredArgs e)
    {
        System.out.println("Spokes_CallAnswered: callid = "+e.CallId+", callsource = "+e.CallSource);
    }
    
    public void Spokes_CallEnded(CallEndedArgs e)
    {
        System.out.println("Spokes_CallEnded: callid = "+e.CallId+", callsource = "+e.CallSource);
    }
    
    public void Spokes_CallSwitched(EventArgs e)
    {
        System.out.println("Spokes_CallSwitched");
    }
    
    public void Spokes_OnCall(OnCallArgs e)
    {
        System.out.println("Spokes_OnCall: incoming? = "+e.Incoming+", state = "+e.State+", source = "+e.CallSource);
    }
    
    public void Spokes_NotOnCall(EventArgs e)
    {
        System.out.println("Spokes_NotOnCall");
    }
    
    public void Spokes_MuteChanged(MuteChangedArgs e)
    {
        System.out.println("Spokes_MuteChanged: muted? = "+e.m_muteon);
    }
    
    public void Spokes_LineActiveChanged(LineActiveChangedArgs e)
    {
        System.out.println("Spokes_LineActiveChanged: lineactive = "+e.m_lineactive);
    }
    
    public void Spokes_Attached(AttachedArgs e)
    {
        System.out.println("Spokes_Attached: productname = "+e.m_device+", prodid = "+e.m_deviceid);
    }
    
    public void Spokes_Detached(EventArgs e)
    {
        System.out.println("Spokes_Detached");
    }
    
    public void Spokes_CapabilitiesChanged(CapabilitiesEventArgs e)
    {
        System.out.println("Spokes_CapabilitiesChanged:");
        System.out.println("HasCallerId = " + e.HasCallerId);
        System.out.println("HasDocking = " + e.HasDocking);
        System.out.println("HasMultiline = " + e.HasMultiline);
        System.out.println("HasProximity = " + e.HasProximity);
        System.out.println("HasWearingSensor = " + e.HasWearingSensor);
        System.out.println("IsWireless = " + e.IsWireless);
    }
    
    public void Spokes_MultiLineStateChanged(MultiLineStateArgs e)
    {
        System.out.println("Spokes_MultiLineStateChanged: line states = ");
        System.out.println("PCActive = " + e.MultiLineState.PCActive);
        System.out.println("PCHeld = " + e.MultiLineState.PCHeld);
        System.out.println("MobileActive = " + e.MultiLineState.MobileActive);
        System.out.println("MobileHeld = " + e.MultiLineState.MobileHeld);
        System.out.println("DeskphoneActive = " + e.MultiLineState.DeskphoneActive);
        System.out.println("IsWireless = " + e.MultiLineState.DeskphoneHeld);
    }
}
