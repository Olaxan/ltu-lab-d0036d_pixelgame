package server;

import org.w3c.dom.css.RGBColor;

import java.awt.*;
import java.awt.image.BufferedImage;
import javax.swing.JPanel;

public class BoardComponent extends JPanel
{

	/**
	 * 
	 */
	private static final long serialVersionUID = 3954213753986046747L;
	
	private int horizontal;
	private int vertical;

	private BufferedImage image;

	public BoardComponent(int h, int v)
	{
		super();

		horizontal = h;
		vertical = v;
		image = new BufferedImage(horizontal, vertical, BufferedImage.TYPE_INT_RGB);
	}

	@Override
	protected void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		Graphics2D graphics2d = (Graphics2D) g;

		graphics2d.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_SPEED);

		int width = getWidth();
		int height = getHeight();
		
		g.drawImage(image, 0, 0, width, height, 0, 0, horizontal, vertical, null);

	}

	public void SetPixelCount(int h, int v)
	{
		horizontal = h;
		vertical = v;
	}
	
	public void SetPixel(int x, int y, int rgb)
	{
		System.out.println(x + ", "+ y + ", " + rgb);


		if (x == -1 || y == -1)
		{
			Graphics2D g = (Graphics2D) image.getGraphics();
			g.setPaint ( new Color(rgb) );
			g.fillRect(0, 0, getWidth(), getHeight());
		}
		else if (x > 0 && x < getWidth() && y > 0 && y < getHeight())
		{
			image.setRGB(x, y, rgb);
		}

		repaint();
	}
}
