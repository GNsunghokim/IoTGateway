package kr.dms.iot.lifx.desktop;

import java.util.ArrayList;

import kr.dms.iot.lifx.desktop.imitating.Context;
import lifx.java.android.client.LFXClient;
import lifx.java.android.entities.LFXHSBKColor;
import lifx.java.android.entities.LFXTypes.LFXFuzzyPowerState;
import lifx.java.android.entities.LFXTypes.LFXPowerState;
import lifx.java.android.light.LFXLight;
import lifx.java.android.light.LFXLight.LFXLightListener;
import lifx.java.android.light.LFXLightCollection;
import lifx.java.android.light.LFXLightCollection.LFXLightCollectionListener;
import lifx.java.android.light.LFXTaggedLightCollection;
import lifx.java.android.network_context.LFXNetworkContext;
import lifx.java.android.network_context.LFXNetworkContext.LFXNetworkContextListener;

public class LifxNonStaticStarter implements LFXLightListener, LFXLightCollectionListener, LFXNetworkContextListener  {
	
	private LFXNetworkContext networkContext;
	private Context imitationContext;
	

	public int startTheLifx()
	{
		imitationContext = new Context();
		networkContext = LFXClient.getSharedInstance(imitationContext ).getLocalNetworkContext();
		networkContext.connect();
		
		// from power activity
		networkContext.addNetworkContextListener( this);
		networkContext.getAllLightsCollection().addLightCollectionListener( this);
		
		/*
		ArrayList<LFXLight> allLights = networkContext.getAllLightsCollection().getLights();
		
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		System.out.println("allLights.size  = " + allLights.size() );
		*/
		return 0;
	}

	@Override
	public void networkContextDidConnect(LFXNetworkContext networkContext) {
		// TODO Auto-generated method stub
		System.out.println("networkContextDidConnect was Triggered");
	}

	@Override
	public void networkContextDidDisconnect(LFXNetworkContext networkContext) {
		// TODO Auto-generated method stub
		System.out.println("networkContextDidDisconnect was Triggered");
	}

	@Override
	public void networkContextDidAddTaggedLightCollection(
			LFXNetworkContext networkContext,
			LFXTaggedLightCollection collection) {
		// TODO Auto-generated method stub
		
		System.out.println("networkContextDidAddTaggedLightCollection was Triggered");
	}

	@Override
	public void networkContextDidRemoveTaggedLightCollection(
			LFXNetworkContext networkContext,
			LFXTaggedLightCollection collection) {
		// TODO Auto-generated method stub
		System.out.println("networkContextDidRemoveTaggedLightCollection was Triggered");
	}

	@Override
	public void lightCollectionDidAddLight(LFXLightCollection lightCollection,
			LFXLight light) {
		// TODO Auto-generated method stub
		System.out.println("lightCollectionDidAddLight was Triggered");
		
		ArrayList<LFXLight> allLights = networkContext.getAllLightsCollection().getLights();
		System.out.println("allLights.size  = " + allLights.size() );
		allLights.get(0).setPowerState(LFXPowerState.ON);
		
		LFXHSBKColor color = LFXHSBKColor.getColor( (float) (0.05 * 360), 1.0f, 0.2f, 3500);
		allLights.get(0).setColor(color);
		
		
	}
	
	public void turnItOFF()
	{
		ArrayList<LFXLight> allLights = networkContext.getAllLightsCollection().getLights();
		System.out.println("allLights.size  = " + allLights.size() );
		allLights.get(0).setPowerState(LFXPowerState.OFF);
		

	}

	@Override
	public void lightCollectionDidRemoveLight(
			LFXLightCollection lightCollection, LFXLight light) {
		// TODO Auto-generated method stub
		System.out.println("lightCollectionDidRemoveLight was Triggered");
	}

	@Override
	public void lightCollectionDidChangeLabel(
			LFXLightCollection lightCollection, String label) {
		// TODO Auto-generated method stub
		System.out.println("lightCollectionDidChangeLabel was Triggered");
	}

	@Override
	public void lightCollectionDidChangeColor(
			LFXLightCollection lightCollection, LFXHSBKColor color) {
		// TODO Auto-generated method stub
		System.out.println("lightCollectionDidChangeColor was Triggered");
	}

	@Override
	public void lightCollectionDidChangeFuzzyPowerState(
			LFXLightCollection lightCollection,
			LFXFuzzyPowerState fuzzyPowerState) {
		// TODO Auto-generated method stub
		System.out.println("lightCollectionDidChangeFuzzyPowerState was Triggered");
	}

	@Override
	public void lightDidChangeLabel(LFXLight light, String label) {
		// TODO Auto-generated method stub
		System.out.println("lightDidChangeLabel was Triggered");
	}

	@Override
	public void lightDidChangeColor(LFXLight light, LFXHSBKColor color) {
		// TODO Auto-generated method stub
		System.out.println("lightDidChangeColor was Triggered");
	}

	@Override
	public void lightDidChangePowerState(LFXLight light,
			LFXPowerState powerState) {
		// TODO Auto-generated method stub
		System.out.println("lightDidChangePowerState was Triggered");
	}

}
