package agentCell_re.models;

import java.awt.AWTException;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Robot;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.Toolkit;
import java.awt.Window;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTabbedPane;
import javax.swing.border.Border;

public class DisplayStandardizer extends JPanel {

	JFrame frame;
	JLabel label;
	static Window activeWindow;

	public DisplayStandardizer(JFrame frame) {
		super(new BorderLayout());
		this.frame = frame;
		label = new JLabel("<html>Activate the 3D Display with amouse click.<br>"
				+ "Bring the Display Standardizer <br>window back on screen.<br>"
				+ "Then hit the \"Standardize!\" button.</html>", JLabel.CENTER);
		String description = "Standardize the 3D Display";

		add(label, BorderLayout.PAGE_END);
		label.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		JButton standardizeButton = new JButton("Standardize!");

		JPanel standardizePanel = createDialogBox();

		// Lay them out.
		Border padding = BorderFactory.createEmptyBorder(20, 20, 5, 20);
		standardizePanel.setBorder(padding);

		JTabbedPane tabbedPane = new JTabbedPane();
		tabbedPane.addTab("Standardize Dialog", null, standardizePanel, description); // tooltip text

		add(tabbedPane, BorderLayout.CENTER);
		add(label, BorderLayout.PAGE_END);
		label.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
	}

	public static void initialize(Window newWindow) {
		//activeWindow = newWindow;
		// Schedule a job for the event-dispatching thread:
		// creating and showing this application's GUI.
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				createAndShowGUI();
			}
		});
	}

	private static void createAndShowGUI() {
		// Create and set up the window.
		JFrame frame = new JFrame("3D Display Standardizer");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Create and set up the content pane.
		DisplayStandardizer newContentPane = new DisplayStandardizer(frame);
		newContentPane.setOpaque(true); // content panes must be opaque
		frame.setContentPane(newContentPane);

		// Display the window.
		frame.pack();
		frame.setVisible(true);
	}

	private JPanel createDialogBox() {

		JButton standardizeButton = new JButton("Standardize it!");
		standardizeButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {

				activeWindow = javax.swing.FocusManager.getCurrentManager().getActiveWindow();
				Window[] windows = Window.getWindows();
				Window aWindow = windows[0];
				//theFactory = aWindow.getComponent(0).getComponent(1).getComponent(0).getComponent(0).getComponent(0);
				// 0 JRootPane, 1 JLayeredPane, 0 JPanel, 0 CContentArea, 0 EfficientControlFactory$3 
				//System.out.println(windows);
				// Component comp = getComponent(0);
			    // Rectangle rect = new Rectangle(comp.getLocationOnScreen(), comp.getSize());
			   
				// My screen is 1600 pixels wide and 900 pixels high.
				// The Display on my screen is of size 1125(width) and 643(height).
				// It starts at 464 211
				// Coordinates for mouse movement
				int xDisplayMiddle = 464+562;
				int yDisplayTop = 300;
				int xCoord = xDisplayMiddle;
				int yCoord = yDisplayTop;

				Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
				System.out.println("In pixels this screen's height is: "+ screenSize.height + " and this screen's width is: "+ screenSize.width);
				
				// The number of notches the mouse whell is turned
				// The effect depends on your hardware.
				// So, customize this value
				int mouseWheelNotches = 15; // The number of notches

				// The shift in y direction to accomplish rotation
				int rotatingShift = 100;
				
				//The tube is 10^6 units long (zdim).
				//We need to drag the mouse pressed from top to bottom
				//until we reach z-value 100.
				//The view starts in the middle, therefore z-value 500,000.
				//int dragShift = 2;

				Robot robot = null;
				for(int i=0; i<1; i++) {
				try {
					robot = new Robot();

					// The standardization action

					// Move the mouse into the 3D Display
					robot.mouseMove(xCoord, yCoord);

					// Zoom out of the 3D Display
					//robot.mouseWheel(mouseWheelNotches);
					
						// Rotate 3D Display
						robot.mousePress(InputEvent.BUTTON3_DOWN_MASK);
						yCoord = yCoord + rotatingShift;
						robot.delay(500);
						robot.mouseMove(xCoord, yCoord);
						robot.mouseRelease(InputEvent.BUTTON3_DOWN_MASK);
						//robot.delay(100);
						//robot.mouseMove(xDisplayMiddle, yDisplayTop);
					
					/*
					//Drag with Right-Click to get to bacteria position
					robot.mousePress(InputEvent.BUTTON3_DOWN_MASK);
					robot.delay(1000);
					yCoord = yCoord + dragShift;
					robot.mouseMove(xCoord, yCoord);
					robot.delay(500);
					robot.mouseRelease(InputEvent.BUTTON3_DOWN_MASK);
					robot.delay(100);
					*/
					
				} catch (AWTException e1) {
					e1.printStackTrace();
				}
				
				}

				return;
			}
		});

		return createPane("3D Display Standardizer:", standardizeButton);
	}

	private JPanel createPane(String description, JButton standardizeButton) {

		JPanel box = new JPanel();
		JLabel label = new JLabel(description);

		box.setLayout(new BoxLayout(box, BoxLayout.PAGE_AXIS));
		box.add(label);

		JPanel pane = new JPanel(new BorderLayout());
		pane.add(box, BorderLayout.PAGE_START);
		pane.add(standardizeButton, BorderLayout.PAGE_END);
		return pane;
	}
}
