package server;

import java.io.IOException;
import java.net.*;
import java.util.Vector;

interface OnReceivedEventListener
{
	void onReceivedEvent(BoardPacket packet);
}

public class BoardListener implements Runnable
{
	private int port;
	private DatagramSocket socket;
	private byte[] buf = new byte[256];

	private Vector<OnReceivedEventListener> onReceivedListeners;

	public BoardListener(int port) throws SocketException, UnknownHostException
	{
		this.port = port;
		this.socket = new DatagramSocket(port, InetAddress.getByName("::1"));
		onReceivedListeners = new Vector<OnReceivedEventListener>();
	}
	
	private void broadcastOnReceived(BoardPacket packet)
	{
		System.out.println("Packet sent to " + onReceivedListeners.size() + " subscribers");
		
		for (OnReceivedEventListener listener : onReceivedListeners)
		{
			listener.onReceivedEvent(packet);
		}
	}

	public void addOnReceivedListener(OnReceivedEventListener listener)
	{
		onReceivedListeners.add(listener);
	}

	public void removeOnReceivedListener(OnReceivedEventListener listener)
	{
		onReceivedListeners.remove(listener);
	}

	public void startListening() throws IOException
	{
		while (true)
		{
			DatagramPacket request = new DatagramPacket(buf, buf.length);
			socket.receive(request);
			
			try
			{
				System.out.println("Received packet!");
				BoardPacket bp = BoardPacket.deserializeBoardPacket(request.getData());
				broadcastOnReceived(bp);
			}
			catch (IOException e)
			{
				System.err.println("Discarded malformed packet: " + e.getMessage());
			}
		}
	}

	@Override
	public void run()
	{
		try
		{
			System.out.println("Server is running on port " + port);
			startListening();
		} 
		catch (IOException e)
		{
			System.err.println("Failed to bind to port " + port + "\n" + e.getMessage());
		}
	}
}
