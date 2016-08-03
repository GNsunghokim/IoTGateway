//
//  LFXTimerUtils.java
//  LIFX
//
//  Created by Jarrod Boyes on 24/03/14.
//  Copyright (c) 2014 LIFX Labs. All rights reserved.
//

package lifx.java.android.util;

import java.util.Timer;
import java.util.TimerTask;

/*rustamchange// android specific codes off
import android.os.Handler;
import android.os.Looper;
*/

public class LFXTimerUtils
{
	private static class MainThreadRunnableTimerTask extends TimerTask
	{
		private Runnable task;
		
		public MainThreadRunnableTimerTask( Runnable task)
		{
			this.task = task;
		}
		
		@Override
		public void run()
		{
			/* rustamchange// change it to thread
			Handler handler = new Handler( Looper.getMainLooper());
			handler.post( task);
			*/
			Thread threadjon = new Thread(task);
			threadjon.start();
		}
	}
	
	public static Timer getTimerTaskWithPeriod( Runnable task, long period, boolean immediate)
	{		
		Timer timer = new Timer();
		long delay = 0L;
		
		if( !immediate)
		{
			delay = period;
		}
		
		timer.scheduleAtFixedRate( new MainThreadRunnableTimerTask( task), delay, period);
	    return timer;
	}
	
	public static void scheduleDelayedTask( Runnable task, long delay)
	{
		/*rustamchange// change it to thread
		Handler handler = new Handler( Looper.getMainLooper());
		handler.postDelayed( task, delay);
		*/
		Thread threadjon = new Thread(task);
		try {
			Thread.sleep(delay);
			threadjon.start();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
}
