//Simple class used to test the java api

import java.io.*;
import java.util.*;
import java.nio.file.*;
import edu.rutgers.winlab.jmfapi.*;

class Receiver{
	private static void usage(){
		System.out.println("Usage:");
		System.out.println("receiver [<src_GUID>]");
	}
	public static void main(String []argv){
		String scheme = "basic";
		GUID srcGUID = null;
                GUID myGUID = null;
		int i = 0;
		if(argv.length >= 1) srcGUID = new GUID(Integer.parseInt(argv[0]));
                if(argv.length >=2) myGUID = new GUID(Integer.parseInt(argv[1]));

		Path file = FileSystems.getDefault().getPath("temp.txt");
		try{
			Files.createFile(file);
		} catch(IOException e){
			try{
				Files.delete(file);
				Files.createFile(file);
			} catch(IOException e2){
				return;
			}
		}
		byte[] buf = new byte[1000000];
		int ret;
		JMFAPI receiver = new JMFAPI();
		try{
                        GUID[] guids = new GUID[1];
                        guids[0] = myGUID;
			if(srcGUID!=null) receiver.jmfopen(scheme, srcGUID);
			else receiver.jmfopen(scheme);
                        receiver.jmfattach(guids);
			while(i < 24954287){
				ret = receiver.jmfrecv_blk(null, buf, 1000000);
                                System.out.println(ret);
				try{
					Files.write(file, buf, StandardOpenOption.APPEND);
				} catch (IOException e){
					System.out.println(e.toString());
				}
				i += ret;

			}
			receiver.jmfclose();
		} catch (JMFException e){
			System.out.println(e.toString());
		}
		System.out.println("Transfer completed");
	}
}
