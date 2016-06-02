//Simple class used to test the java api

import java.io.*;
import java.util.*;
import java.nio.file.*;

//Import MF Socket API
import edu.rutgers.winlab.jmfapi.*;

class Receiver{
	private static void usage(){
		System.out.println("Usage:");
		System.out.println("receiver [<src_GUID>]");
	}
	
	public static void main(String []argv){
		//The profile describes the nature of the communication that will follow and
		//	is used by the network stack to select the best end-to-end transport
		//For this application a 'basic' profile is selected providing only a message based
		//	transport with no added realiability on top of what offered by the network.
		String scheme = "basic";
		
		GUID srcGUID = null;
		GUID senderGUID = new GUID();
		int i = 0;
		
		//A GUID class is used for name based communications
		//The source is optional. If a source is not specified, the default GUID of the device is used
		if(argv.length == 1) srcGUID = new GUID(Integer.parseInt(argv[0]));
		
		Path file = FileSystems.getDefault().getPath("temp.txt");
		try{
			Files.createFile(file);
		} catch(IOException e){
			try{
				Files.delete(file);
				Files.createFile(file);
			} catch(IOException e2){
				System.out.println(e2.toString());
				return;
			}
		}
		
		byte[] buf = new byte[1000000];
		int ret;
		JMFAPI receiver = new JMFAPI();
		try{
			if(srcGUID!=null) receiver.jmfopen(scheme, srcGUID);
			else receiver.jmfopen(scheme);
			
			//First message will include the size of the transfered file
			ret = receiver.jmfrecv_blk(senderGUID, buf, 1000000);
			int fileSize = Utils.byteArrayToInt(buf, 0);
			System.out.println("I will receive a file of size " + fileSize + " bytes from host with GUID " + senderGUID.getGUID());
			
			int total = 0;
			while(i < fileSize){
				ret = receiver.jmfrecv_blk(null, buf, 1000000);
				total+=ret;
				System.out.println("Received " + ret + " bytes");
				try{
					Files.write(file, Arrays.copyOfRange(buf, 0, ret), StandardOpenOption.APPEND);
				} catch (IOException e){
					System.out.println(e.toString());
				}
				i += ret;

			}
			
			//Send back an acknowledgement with the amount of bytes received
			byte[] answer = Utils.intToByteArray(total);
			receiver.jmfsend(answer,4, senderGUID);
			
			receiver.jmfclose();
		} catch (JMFException e){
			System.out.println(e.toString());
		}
		System.out.println("Transfer completed");
	}
}

