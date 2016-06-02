package edu.umass.cs.msocket.apps;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.UnknownHostException;
import java.time.Duration;
import java.util.concurrent.*;

import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.mobility.MobilityManagerClient;


public class MSocketClient
{
	public static void main(String[] args) throws IOException, InterruptedException, ExecutionException
	{

		String serverName = args[0];	
		//String strGUID = args[1];
		String mGUID = "102";
		String oGUID = "101";
		
		ExecutorService executor = Executors.newSingleThreadExecutor();
        Future<String> future = executor.submit(new Task(serverName, mGUID, oGUID));
        try {
            System.out.println("Started..");
            System.out.println(future.get(3, TimeUnit.SECONDS));
            System.out.println("Finished!");
        } catch (TimeoutException e) {
            future.cancel(true);
            System.out.println("Terminated!");
        }
		

	}
}

class Task implements Callable<String> {
    private String serverName;
	private String mGUID;
	private String oGUID;
	private MSocket msock;
	
	public Task(String serverName, String mGUID, String oGUID) throws UnknownHostException, IOException{
    	this.serverName = serverName;
    	this.mGUID = mGUID;
    	this.oGUID = oGUID;
    	
    	final byte[] myGUID = new byte[20];
		System.arraycopy(mGUID.getBytes(), 0, myGUID, 20-mGUID.length(), mGUID.length());
	  	  
		final byte[] otherGUID = new byte[20];
		System.arraycopy(oGUID.getBytes(), 0, otherGUID, 20-oGUID.length(), oGUID.length());
	  	  
		this.msock = new MSocket(serverName, 0, myGUID, otherGUID, 0);
    }
    @Override
    public String call() throws Exception {
	    // Do your long running task here.
		OutputStream outstream = msock.getOutputStream();
		InputStream inpstream = msock.getInputStream();
		byte[] byteArray = new byte[1000];
		int i=0;
		
    	while (!Thread.interrupted()) {
    		while(i < 6)
    		{
    			//outstream.write( new String("hello world from client").getBytes() );
    			inpstream.read(byteArray);
    			System.out.println("trying to read");
    			System.out.println(new String(byteArray));
    			
    			try
    			{
    				Thread.sleep(2000);
    			} catch (InterruptedException e)
    			{
    				e.printStackTrace();
    			}
    			
    			i++;
    		}
    		msock.close();
    		MobilityManagerClient.shutdownMobilityManager();
    		return "Ready";
    	}
    	
    	msock.close();
		MobilityManagerClient.shutdownMobilityManager();
        return "Ready!";
    }
}