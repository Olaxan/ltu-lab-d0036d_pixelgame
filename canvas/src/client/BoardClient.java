package client;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Random;
import java.util.Scanner;

import server.BoardPacket;

public class BoardClient
{	
	
	static Random rand = new Random();
	
	public static void main(String[] args) throws IOException
	{
		
		System.out.println("Board client - enter command:");
		System.out.println("x, y, rgb (exit to exit)\n");
		
		DatagramSocket socket = new DatagramSocket();
		Scanner scan = new Scanner(System.in);
	    InetAddress address = InetAddress.getByName("::1");
		
		while (true)
		{
			String cmd = scan.nextLine();
			String[] cmds = cmd.split("[,\\s]+");
			
			if (cmd.equals("exit"))
				break;
			
			if (cmds[0].equals("random"))
			{
				if (cmds.length == 4)
				{
					int h, v, c;
					
					try
					{
						h	= Integer.parseInt(cmds[1]);
						v	= Integer.parseInt(cmds[2]);
						c	= Integer.parseInt(cmds[3]);
					}
					catch (NumberFormatException e)
					{
						System.out.println("Invalid random parameters - enter 'random horizontal, vertical, count'");
						continue;
					}
					
					for (int i = 0; i < c; i++)
					{
						BoardPacket pack = new BoardPacket(rand.nextInt(h), rand.nextInt(v), rand.nextInt(Integer.MAX_VALUE));
						byte[] bytes = pack.serializeBoardPacket();
						DatagramPacket packet = new DatagramPacket(bytes, bytes.length, address, 4445);
						socket.send(packet);
					}
				}
				else
					System.out.println("Invalid parameter count - 'random horizontal, vertical, count'");
				
				continue;
			}
			
			if (cmds.length == 3)
			{
				int x, y, rgb;
				
				try
				{
					x 	= Integer.parseInt(cmds[0]);
					y 	= Integer.parseInt(cmds[1]);
					rgb = Integer.parseInt(cmds[2]);					
				}
				catch (NumberFormatException e)
				{
					System.out.println("Invalid command - 'enter x, y, rgb'");
					continue;
				}
				
				BoardPacket pack = new BoardPacket(x, y, rgb);
				byte[] bytes = pack.serializeBoardPacket();
				DatagramPacket packet = new DatagramPacket(bytes, bytes.length, address, 4445);
		        socket.send(packet);
				
			}
			else
				System.out.println("Invalid parameter count - 'enter x, y, rgb'");
		}
		
		socket.close();
		scan.close();
	}
}
