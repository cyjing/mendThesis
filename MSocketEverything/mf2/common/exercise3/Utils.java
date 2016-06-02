public class Utils {

	public static int byteArrayToInt(byte buffer[], int pos){
		int ret = 0;
		ret += buffer[pos] & 0x000000FF << 24;
		ret += (buffer[pos+1] & 0x000000FF) << 16;
		ret += (buffer[pos+2] & 0x000000FF) << 8;
		ret += (buffer[pos+3] & 0x000000FF);
		return ret;
	}

	public static byte[] intToByteArray(int a) {
		byte[] ret = new byte[4];
		ret[3] = (byte) (a & 0xFF);
		ret[2] = (byte) ((a >> 8) & 0xFF);
		ret[1] = (byte) ((a >> 16) & 0xFF);
		ret[0] = (byte) ((a >> 24) & 0xFF);
		return ret;
	}
}

