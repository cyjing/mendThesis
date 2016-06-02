//Simple class used to test the java api


//jmfapi needs to be in the classpath
import java.io.*;
import java.util.*;
import java.nio.file.*;
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
		String scheme = "basic";
		GUID srcGUID = null, dstGUID;
		int na = 0;
		dstGUID = new GUID(Integer.parseInt(argv[1]));
		if(argv.length == 3) srcGUID = new GUID(Integer.parseInt(argv[2]));
		if(argv.length == 4) na = Integer.parseInt(argv[3]);
		Path file = FileSystems.getDefault().getPath(argv[0]);
		JMFAPI sender = new JMFAPI();
		try{
			if(srcGUID!=null) sender.jmfopen(scheme, srcGUID);
			else sender.jmfopen(scheme);
			byte[] fileArray;
			try {
				fileArray = Files.readAllBytes(file);
			} catch (IOException e){
				System.out.println("ERROR");
				return;
			}


			byte[] tempArray = new byte[1000000];
			int ret, read = 0;
	     	        tempArray = Arrays.copyOfRange(fileArray, 0, fileArray.length - read - 1);
                        for(int i =0; i<10; i++){
			    sender.jmfsend(tempArray,1000000, dstGUID, 0);
                        }
                        sender.jmfclose();
			System.out.println("Transfer completed");
		} catch (JMFException e){
			System.out.println(e.toString());
		}
	}
}
