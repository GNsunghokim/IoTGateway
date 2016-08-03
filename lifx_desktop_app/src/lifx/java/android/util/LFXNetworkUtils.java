//
//  LFXNetworkUtils.java
//  LIFX
//
//  Created by Jarrod Boyes on 24/03/14.
//  Copyright (c) 2014 LIFX Labs. All rights reserved.
//

package lifx.java.android.util;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.List;


//rustamchange//import android.content.Context;
import kr.dms.iot.lifx.desktop.imitating.Context;

/* rustamchange// try to turn down the errors
import org.apache.http.conn.util.InetAddressUtils;


import android.annotation.SuppressLint;
import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiManager;
*/


public class LFXNetworkUtils
{
	/* rustamchahge// rewrite the getLocalHostAddress for PC version, for the beginning
	 * just start with static local IP
	@SuppressLint( "DefaultLocale")
	public static String getLocalHostAddress() 
	{
		boolean useIPv4 = true;
		
        try 
        {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) 
            {
                List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
                for (InetAddress addr : addrs) 
                {
                    if (!addr.isLoopbackAddress()) 
                    {
                        String sAddr = addr.getHostAddress().toUpperCase();
                        boolean isIPv4 = InetAddressUtils.isIPv4Address(sAddr); 
                        if (useIPv4) 
                        {
                            if (isIPv4) 
                            {
                                return sAddr;
                            }
                        } 
                        else 
                        {
                            if (!isIPv4) 
                            {
                                int delim = sAddr.indexOf('%'); // drop ip6 port suffix
                                return delim<0 ? sAddr : sAddr.substring(0, delim);
                            }
                        }
                    }
                }
            }
        } 
        catch (Exception ex) { } // for now eat exceptions
        return "";
    }
    */
	//rustamchange// for the beginning lets start with static IP
	public static String getLocalHostAddress() 
	{
		return "192.168.25.51";
	}
	
	
//	public static String getBroadcastAddress()
//	{
//		return "255.255.255.255";
//	}
	
//	public static String getBroadcastAddress()
//	{
//	    String found_bcast_address = null;
//	    System.setProperty( "java.net.preferIPv4Stack", "true"); 
//	    
//	    try
//	    {
//	         Enumeration<NetworkInterface> niEnum = NetworkInterface.getNetworkInterfaces();
//	         while( niEnum.hasMoreElements())
//	         {
//	             NetworkInterface ni = niEnum.nextElement();
//	             
//	             Enumeration<InetAddress> iNetAddresses = ni.getInetAddresses();
//	             while( iNetAddresses.hasMoreElements())
//	             {
//	            	 InetAddress i = iNetAddresses.nextElement();
//	            	 System.out.println( "Address: " + i.getHostAddress());
//	             }
//	             
//	             if( !ni.isLoopback())
//	             {
//	                for( InterfaceAddress interfaceAddress : ni.getInterfaceAddresses())
//	                {
//	                	if( interfaceAddress.getBroadcast() != null)
//	                	{
//	                		found_bcast_address = interfaceAddress.getBroadcast().toString();
//	                		found_bcast_address = found_bcast_address.substring( 1);
//	                	}
//	                	else
//	                	{
//	                		System.out.println( "Found bcast address: " + interfaceAddress.toString());
//	                	}
//	                }
//	             }
//	          }
//	     }
//	     catch( SocketException e)
//	     {
//	          e.printStackTrace();
//	     }
//
//	     return found_bcast_address;
//	}
	
	/*rustamchange// this broadcast address function also android specific, so its turned into static too
	public static String getBroadcastAddress( Context context)
	{
	    WifiManager wifi = (WifiManager) context.getSystemService( Context.WIFI_SERVICE);
	    DhcpInfo dhcp = wifi.getDhcpInfo();

	    int broadcast = (dhcp.ipAddress & dhcp.netmask) | ~dhcp.netmask;
	    byte[] quads = new byte[4];
	    for (int k = 0; k < 4; k++)
	      quads[k] = (byte) (broadcast >> (k * 8));
	    try
		{
			return InetAddress.getByAddress(quads).getHostAddress();
		} 
	    catch( UnknownHostException e)
		{
			e.printStackTrace();
		}
	    
	    return "255.255.255.255";
	}
	*/
	
	//rustamchange// getBroadcastAddress function is into static one
	public static String getBroadcastAddress(Context context)
	{
		//return "255.255.255.255";
		return "192.168.0.255";
	}
	
	public static String getIPv4StringByStrippingIPv6Prefix( String in)
	{
		String ipv6Prefix = "::ffff:";
		
		if( in.startsWith( ipv6Prefix)) 
		{
			return in.substring( ipv6Prefix.length(), in.length());
		}
		
		return in;
	}
}
