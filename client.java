import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class bclient extends Application{
	
static int PORT = 2000;
static Socket socket;
static OutputStreamWriter out;
Writer writer;
ArrayList<String> input = new ArrayList<String>(); //From keyboard
public static ArrayList<String> commands = new ArrayList<String>(); //From server
static Thread tcpListen;
ArrayList<sprites> players = new ArrayList<sprites>(); //add player dynamically


public static class listen extends Thread
{
	public void run()
	{
		try
		{
			System.out.println("Setup TCP");
			socket = new Socket("192.168.1.3",PORT);
			System.out.printf("Connected to HOST IP: %s and PORT: %s\n",socket.getInetAddress(),socket.getPort());
			InputStreamReader in = new InputStreamReader(socket.getInputStream());
			BufferedInputStream buffin = new BufferedInputStream(socket.getInputStream());
			
			
			//send ip and port
			
			OutputStreamWriter out1 = new OutputStreamWriter(socket.getOutputStream());
			char[] writeBuffer = new char[256];
			
			char[] readBuffer = new char[256];

			int lastindx=0;
			
		
			
			while(in.read(readBuffer)!=-1)
			{
				
				//basically cut message down from 256 to actual message
				for(int i = 0; i<256; i++)
				{
					if(readBuffer[i] == 0)
					{
						lastindx=i;
						break;
					}
				}

				
				//make string out of message
				String bufferRead = new String(readBuffer,0,8);
				//add to commandArray
				commands.add(bufferRead);
				System.out.printf("server says: ");
				System.out.println(bufferRead);
				

				if(bufferRead.equals("malin"))
				{
					System.out.println("YEHA BABY WE GOT IT");
				}
				
				//Sending out ID number to our server thread
				String clientid = "ID:";
				String check[] = bufferRead.split(":");
				
				if(check[0].equals("clntid"))
				{
					clientid += check[1];
					System.out.printf("Our ID num: %s\n", clientid);					
					writeBuffer = clientid.toCharArray();
					out1.write(clientid);
					out1.flush();
				}
				Arrays.fill(readBuffer, (char)0); //clear 
				System.out.println("cleared");
			}
			
		} catch(IOException e) {
			System.out.println("Can't connect to host");
			
		}
		
	}
	
	
}



	@Override
	public void start(Stage primaryStage) throws Exception
	
	{
		//some javafx action
		Group root = new Group();
		Scene game = new Scene(root);
		Canvas canvas = new Canvas(1280,720);
		root.getChildren().add(canvas);
		GraphicsContext gc = canvas.getGraphicsContext2D();
		gc.setFill(Color.BLUE);
		gc.fillOval(10, 60, 30, 30);
		
		
		//Set player
		sprites player = new sprites();
		player.setImage("yrthuvud.png");
		player.setPostition(640, 360);
		player.setVelocity(0, 0);
		
		//init playerlist
		sprites temp = new sprites();
		//just to fill the playes, these will get moved as players join
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		players.add(temp);
		
		
		
		
		AnimationTimer timer = new AnimationTimer() 
		
		{
			
			@Override
			public void handle(long now)
			{
				char[] writeBuffer = new char[256];
				
				try {
					//if error here check server ifconfig if right IP
					out = new OutputStreamWriter(socket.getOutputStream());
				} catch (IOException e) {
					System.out.println("Error at out");
					e.printStackTrace();
				}
				

				//HANDLE JOIN
				
				for(int i=0;i<commands.size();i++)
				{
					if(commands.get(i).contains("JOINSE"))
					{
						//Dynamically add the player to the game
						//Set client in the array with ID 
						
						System.out.println("got join command indx");
						String [] crID = commands.get(i).split(":");
						System.out.println(Arrays.toString(crID));
						int cID = Integer.parseInt(crID[1]);
						sprites client = new sprites();
						players.remove(cID);
						players.add(cID, client);
						client.setImage("yrthuvud.png");
						client.setPostition(640, 360);
						client.setVelocity(0, 0);
						commands.remove(i);
						break;
						
					}
				}
				//So it doesnt add 4ever
				for(sprites spr : players)
				{
					spr.setVelocity(0, 0);
				}
				//other player right
				for(int i=0;i<commands.size();i++)
				{
					if(commands.get(i).contains("CRIGHT"))
					{
						System.out.println("I entered cright");
						
						String [] crID = commands.get(i).split(":");
						System.out.println(Arrays.toString(crID));	
						int cID = Integer.parseInt(crID[1]);
						players.get(cID).addVelocity(3.5, 0);
						commands.remove(i);
						break;
					}
				}
				
				//other player left
				for(int i=0;i<commands.size();i++)
				{
					if(commands.get(i).contains("CLEFTT"))
					{
						
						String [] crID = commands.get(i).split(":");
						int cID = Integer.parseInt(crID[1]);
						players.get(cID).addVelocity(-3.5, 0);
						commands.remove(i);
						break;
					}
				}
				
				//other player UP
				for(int i=0;i<commands.size();i++)
				{
					if(commands.get(i).contains("CUUPPP"))
					{
						
						String [] crID = commands.get(i).split(":");
						int cID = Integer.parseInt(crID[1]);
						players.get(cID).addVelocity(0,-3.5);
						commands.remove(i);
						break;
					}
				}
				
				//other player DOWN
				for(int i=0;i<commands.size();i++)
				{
					if(commands.get(i).contains("CDOWNN"))
					{
						
						String [] crID = commands.get(i).split(":");
						int cID = Integer.parseInt(crID[1]);
						players.get(cID).addVelocity(0, 3.5);
						commands.remove(i);
						break;
					}
				}
				
				
				
				//So it doesnt add forever set 0,0
				player.setVelocity(0, 0);
				
				if(input.contains("LEFT"))
				{
					String send = "LEFT";
					writeBuffer = send.toCharArray();
					try 
					{
						out.write(send);
						out.flush();
					} catch (IOException e) 
					{
						e.printStackTrace();
					}
					player.addVelocity(-3.5, 0);
					//NEED IF STATEMENT HERE	
//					if(commands.contains("LEFTOK"))
//					{
//						player.addVelocity(-3.5, 0);
//					}commands.remove("LEFTOK");
//					
				}
				if(input.contains("RIGHT"))
				{
					String send = "RIGHT";
					writeBuffer = send.toCharArray();
					try 
					{
						out.write(send);
						out.flush();
					} catch (IOException e) 
					{
						e.printStackTrace();
					}
					player.addVelocity(3.5, 0);
					//IF STATEMENT HERE
//					if(commands.contains("RIGHTOK"))
//					{
//						player.addVelocity(3.5, 0);
//					}commands.remove("RIGHTOK");
					
				}
				if(input.contains("UP"))
				{
					String send = "UP";
					writeBuffer = send.toCharArray();
					try 
					{
						out.write(send);
						out.flush();
					} catch (IOException e) 
					{
						e.printStackTrace();
					}
					player.addVelocity(0, -3.5);
					//IF STATEMENT HERE
//					if(commands.contains("UPOK"))
//					{
//						
//					}commands.remove("UPOK");
//					
				}
				if(input.contains("DOWN"))
				{
					String send = "DOWN";
					writeBuffer = send.toCharArray();
					try 
					{
						out.write(send);
						out.flush();
					} catch (IOException e) 
					{
						e.printStackTrace();
					}
					player.addVelocity(0,3.5);
					
					//IF STATEMENT HERE	
//					if(commands.contains("DOWNOK"))
//					{
//						
//					}commands.remove("DOWNOK");
//					
				}
				
				for(sprites p : players)
				{
					p.update();
				}
				
				if(player.update()==1)
				{
					player.setImage("ledshuvud.png");
				}
				
				//Clear and draw 
				gc.clearRect(0, 0, 1280, 720);
				player.render(gc);
				
				//render all players
				for(sprites pp : players)
				{
					pp.render(gc);
		
				}
				
				
			}
		};
		
		
		
		
		

		
		primaryStage.setTitle("The Shahin Game");
		primaryStage.setScene(game);
		primaryStage.show();
		timer.start();
		
		
		//interrupts on keymove
		
		
		game.setOnKeyPressed(e->{
			String key = e.getCode().toString();
			if(!input.contains(key)) {
				input.add(key);
			}
			
			
		});
		
		game.setOnKeyReleased(e->{
			String key = e.getCode().toString();
			input.remove(key);
			
		});
		
		
		
	}
	
	
	
	

	public static void main(String[] args) 
	{
		tcpListen = new listen();
		tcpListen.start();
		System.out.println("Thread started");
		launch(args);

	}

}
