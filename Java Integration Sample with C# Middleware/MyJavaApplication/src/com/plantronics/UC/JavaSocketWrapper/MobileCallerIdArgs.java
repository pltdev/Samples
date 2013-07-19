/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.plantronics.UC.JavaSocketWrapper;

/**
 * Event arguments for MobileCallerId event handler
 * @author lcollins
 */
public class MobileCallerIdArgs extends EventArgs 
{
    public String MobileCallerId;

    public MobileCallerIdArgs(String mobilecallerid)
    {
        MobileCallerId = mobilecallerid;
    }
}
