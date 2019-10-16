package server;

import java.io.*;

public class BoardPacket implements Serializable
{

	/**
	 * 
	 */
	private static final long serialVersionUID = -3235908484463477345L;
	
	int x;
	int y;
	int rgb;
	
	public BoardPacket(int x, int y, int rgb)
	{
		this.x = x;
		this.y = y;
		this.rgb = rgb;
	}
	
	public byte[] serializeBoardPacket() throws IOException
	{
		byte[] bytes;
		ByteArrayOutputStream bos = new ByteArrayOutputStream();
		DataOutput out = null;
		try
		{
			out = new DataOutputStream(bos);
			out.writeInt(x);
			out.writeInt(y);
			out.writeInt(rgb);
			bytes = bos.toByteArray();
		}
		finally
		{
			try
			{
				bos.close();
			}
			catch (IOException ex) { }
		}
		return bytes;
	}
	
	public static BoardPacket deserializeBoardPacket(byte[] bytes) throws IOException
	{
		BoardPacket obj = new BoardPacket(0,0,0);
		ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
		DataInput in = null;
		try
		{
			in = new DataInputStream(bis);
			obj.x = in.readInt();
			obj.y = in.readInt();
			obj.rgb = in.readInt();
		}
		finally
		{
			try
			{
				if (in != null)
				{
					bis.close();
				}
			}
			catch (IOException ex) { }
		}
		
		return obj;
	}
}
