/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

/**
 *
 * @author lcollins
 */
public interface ISpokesEvents {
    public void Spokes_TakenOff(WearingStateArgs e);
    public void Spokes_PutOn(WearingStateArgs e);
    public void Spokes_Near(EventArgs e);
    public void Spokes_Far(EventArgs e);
    public void Spokes_InRange(EventArgs e);
    public void Spokes_OutOfRange(EventArgs e);
    public void Spokes_Docked(DockedStateArgs e);
    public void Spokes_UnDocked(DockedStateArgs e);
    public void Spokes_MobileCallerId(MobileCallerIdArgs e);
    public void Spokes_OnMobileCall(OnMobileCallArgs e);
    public void Spokes_NotOnMobileCall(EventArgs e);
    public void Spokes_SerialNumber(SerialNumberArgs e);
    public void Spokes_CallAnswered(CallAnsweredArgs e);
    public void Spokes_CallEnded(CallEndedArgs e);
    public void Spokes_CallSwitched(EventArgs e);
    public void Spokes_OnCall(OnCallArgs e);
    public void Spokes_NotOnCall(EventArgs e);
    public void Spokes_MuteChanged(MuteChangedArgs e);
    public void Spokes_LineActiveChanged(LineActiveChangedArgs e);
    public void Spokes_Attached(AttachedArgs e);
    public void Spokes_Detached(EventArgs e);
    public void Spokes_CapabilitiesChanged(CapabilitiesEventArgs e);
    public void Spokes_MultiLineStateChanged(MultiLineStateArgs e);
}
