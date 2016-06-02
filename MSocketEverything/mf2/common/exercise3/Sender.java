//Simple class used to test the java api


//jmfapi needs to be in the classpath
import java.io.*;
import java.util.*;
import java.nio.file.*;

//Import MF Socket API
import edu.rutgers.winlab.jmfapi.*;
import edu.rutgers.winlab.jmfapi.GUID;

class Sender{
	private static void usage(){
		System.out.println("Usage:");
		System.out.println("sender <file> <dst_GUID> [<src_GUID>]");
	}
	
	public static void main(String []argv){
		if(argv.length < 2){
			usage();
			return;
		}
		
		//The profile describes the nature of the communication that will follow and
		//	is used by the network stack to select the best end-to-end transport
		//For this application a 'basic' profile is selected providing only a message based
		//	transport with no added realiability on top of what offered by the network.
		String profile = "basic";
		GUID srcGUID = null;
		
		//A GUID class is used for name based communications
		//The destination of the fie has been passed as a parameter
		GUID dstGUID = new GUID(Integer.parseInt(argv[1]));
		//The source is optional. If a source is not specified, the default GUID of the device is used
		if(argv.length == 3) srcGUID = new GUID(Integer.parseInt(argv[2]));
		
		Path file = FileSystems.getDefault().getPath(argv[0]);
		
		//The JMFAPI object represents the socket and the API to interact with it
		JMFAPI sender = new JMFAPI();
		
		try{
			
			//The open call creates the communication socket and initializes the resources
			if(srcGUID!=null) sender.jmfopen(profile, srcGUID);
			else sender.jmfopen(profile);
			
			byte[] fileArray;
			try {
				fileArray = Files.readAllBytes(file);
			} catch (IOException e){
				System.out.println("ERROR");
				return;
			}
			System.out.println("Transferring a file of size " + fileArray.length);
			byte[] sizeArray = Utils.intToByteArray(fileArray.length);
			sender.jmfsend(sizeArray, 4, dstGUID);
			int sentBytes;
			
			byte[] tempArray;
			int ret, read = 0;
			int bytesToSend = fileArray.length;
			while(bytesToSend>1000000){
				tempArray = Arrays.copyOfRange(fileArray, 0, 999999);
				//Messages are sent up to 10MB at a time (which is the default buffer size for the socket)
				sentBytes = sender.jmfsend(tempArray,1000000, dstGUID);
				bytesToSend -= sentBytes;
				System.out.println("Transmitted " + sentBytes);
			}
			tempArray = Arrays.copyOfRange(fileArray, 0, bytesToSend - 1);
			sentBytes = sender.jmfsend(tempArray,bytesToSend, dstGUID);
			System.out.println("Transmitted " + sentBytes);
			
			//Receive the confirmation from the receiver
			//The first parameter is set to null but could be used to obtain the GUID of the message source
			sender.jmfrecv_blk(null,tempArray, 1000000);
			int receivedBytes = Utils.byteArrayToInt(tempArray, 0);
			System.out.println("The receiver received " + receivedBytes + " succesfully");
			
			//Close the socket and clear the resources
			sender.jmfclose();
			System.out.println("Transfer completed");
			
		} catch (JMFException e){
			//Exceptions related to events occured in the network protocol stack are defined as JMFException
			System.out.println(e.toString());
		}
	}
}

