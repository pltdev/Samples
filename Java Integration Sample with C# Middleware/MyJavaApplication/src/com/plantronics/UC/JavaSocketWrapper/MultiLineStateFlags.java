/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

/**
 * Used with MultiLineStateArgs to hold active/held status of multiple lines (PC, Mobile, Deskphone)
 * @author lcollins
 */
public class MultiLineStateFlags {
    public Boolean PCActive;
    public Boolean MobileActive;
    public Boolean DeskphoneActive;
    public Boolean PCHeld;
    public Boolean MobileHeld;
    public Boolean DeskphoneHeld;
    
    public MultiLineStateFlags(boolean pcactive, boolean pcheld, 
            boolean mobileactive, boolean mobileheld, boolean deskphoneactive,
                boolean deskphoneheld)
    {
        PCActive = pcactive;
        PCHeld = pcheld;
        MobileActive = mobileactive;
        MobileHeld = mobileheld;
        DeskphoneActive = deskphoneactive;
        DeskphoneHeld = deskphoneheld;
    }
}
