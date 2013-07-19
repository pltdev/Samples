/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

/**
 * Event arguments for Capabilities changed event event handler
 * @author lcollins
 */
public class CapabilitiesEventArgs extends EventArgs {
    public boolean HasCallerId = false;
    public boolean HasDocking = false;
    public boolean HasMultiline = false;
    public boolean HasProximity = false;
    public boolean HasWearingSensor = false;
    public boolean IsWireless = false;

    public CapabilitiesEventArgs(boolean hasCallerId, boolean hasDocking,
            boolean hasMultiline, boolean hasProximity,
            boolean hasWearingSensor, boolean isWireless)
    {
        HasCallerId = hasCallerId;
        HasDocking = hasDocking;
        HasMultiline = hasMultiline;
        HasProximity = hasProximity;
        HasWearingSensor = hasWearingSensor;
        IsWireless = isWireless;
    }
}
