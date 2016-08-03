package kr.dms.iot.lifx.desktop.imitating;

public class RusHandler extends Thread {
	
	public void sendMessage(RusMessage msg)
	{
		handleMessage(msg);
	}
	
	public void handleMessage(RusMessage msg) {
		System.out.println("handleMessage is triggered at : RusHandler origin");
    }
}
