package agentCell_re.models;

import java.awt.AWTException;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.border.Border;

import org.jogamp.java3d.BoundingSphere;
import org.jogamp.java3d.BranchGroup;
import org.jogamp.java3d.Canvas3D;
import org.jogamp.java3d.Locale;
import org.jogamp.java3d.Transform3D;
import org.jogamp.java3d.TransformGroup;
import org.jogamp.java3d.exp.swing.JCanvas3D;
import org.jogamp.java3d.utils.universe.SimpleUniverse;
import org.jogamp.java3d.utils.universe.ViewingPlatform;
import org.jogamp.vecmath.Vector3f;

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
		// activeWindow = newWindow;
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
				// aWindow ist ein JFrame.
				// 0 JRootPane,
				// 1 JLayeredPane,
				// 0 JPanel,
				// 0 CContentArea,
				// 0 EfficientControlFactory$3,
				// 1 SplitDockStation$Content,
				// 0 (1=Scenario Tree etc.) BasicDockableDisplayer 9006 (9007),
				// 0 BasicDockableDisplayer$3,
				// 0 CSplitDockStation,
				// 1 SplitDockStation$Content,
				// 0 BasicDockableDisplayer (id9129) hier stehen die ganzen Displays drin,
				// 0 BasicDockableDisplayer$3,
				// 0 JPanel (Hier steht in 1 SmoothDefaultTitle, 0 AbstractDockTitle$1 unter
				// text="agentCell_re: Cells Display" (id9672),
				// 0 JPanel ,
				// 0 JPanel,
				// 0 JPanel,
				// 1 JPanel (0 ist JToolBar),
				// 0 AbstractDisplay3D$ImageablePanel,
				// 0 JCanvas3D -> Universe)

				Container jRootPane = (Container) aWindow.getComponent(0);
				/*
				 * Component jLayeredPane = ((Container) jRootPane).getComponent(1); Component
				 * jPanelA = ((Container) jLayeredPane).getComponent(0); Component cContentArea
				 * = ((Container) jPanelA).getComponent(0); Component efficientControlFactory$3
				 * = ((Container) cContentArea).getComponent(0); Component
				 * splitDockStation$ContentA = ((Container)
				 * efficientControlFactory$3).getComponent(1); Component basicDockableDisplayerA
				 * = ((Container) splitDockStation$ContentA).getComponent(0); Component
				 * basicDockableDisplayer$3A = ((Container)
				 * basicDockableDisplayerA).getComponent(0); Component cSplitDockStation =
				 * ((Container) basicDockableDisplayer$3A).getComponent(0); Component
				 * splitDockStation$ContentB = ((Container) cSplitDockStation).getComponent(1);
				 * Component basicDockableDisplayerB = ((Container)
				 * splitDockStation$ContentB).getComponent(0); Component
				 * basicDockableDisplayer$3B = ((Container)
				 * basicDockableDisplayerB).getComponent(0); Component jPanelB = ((Container)
				 * basicDockableDisplayer$3B).getComponent(0); Component jPanelC = ((Container)
				 * jPanelB).getComponent(0); Component jPanelD = ((Container)
				 * jPanelC).getComponent(0); Component jPanelE = ((Container)
				 * jPanelD).getComponent(0); Component jPanelF = ((Container)
				 * jPanelE).getComponent(1); Component abstractDisplay3D$ImageablePanel =
				 * ((Container) jPanelF).getComponent(0); Component jCanvas3D = ((Container)
				 * abstractDisplay3D$ImageablePanel).getComponent(0);
				 * 
				 * System.out.println(jCanvas3D.getClass());
				 */
				List<JCanvas3D> jCanvases3D = harvestMatches(jRootPane, JCanvas3D.class);
				Canvas3D canvas3D = jCanvases3D.get(0).getOffscreenCanvas3D();

				//JCanvas3D thisJCanvas3D = (JCanvas3D) jCanvas3D;
				//Canvas3D canvas3D = thisJCanvas3D.getOffscreenCanvas3D();
				TransformGroup tG = (TransformGroup) canvas3D.getView().getViewPlatform().getParent();
				ViewingPlatform viewingPlatform = (ViewingPlatform) tG.getParent();
				SimpleUniverse universe = viewingPlatform.getUniverse();
				System.out.println(universe);

				Locale locale = (Locale) universe.getLocale();
				Iterator it = locale.getAllBranchGraphs();
				BranchGroup group = null;
				if(it.hasNext()) {
					group = (BranchGroup)it.next();
				}
				BoundingSphere bounds = (BoundingSphere) group.getBounds();

				TransformGroup viewPlatformTransform = viewingPlatform.getViewPlatformTransform();
				Transform3D trans = new Transform3D();

				// trans.lookAt(new Point3d(0, 0, bounds.getRadius() * 2.5),
				// new Point3d(0, 0, 0), new Vector3d(0, 1, 0));

				// trans.lookAt(new Point3d(0, 0, bounds.getRadius() * 2.5),
				// new Point3d(0, 0, 0), new Vector3d(0, 1, 0));
				// trans.invert();

				Vector3f translate = new Vector3f();
				Transform3D T3D = new Transform3D();
				// y is the value of how far you want to move along the tube.
				// 1 y is roughly equal to 400 mikrometers
				// To calculate how much the camera needs to zoom back in or out
				// after moving in y direction of the camera (along the tube)
				// put the y value in the following formula:
				// z = -2.14*y + 2.41
				// and put the y and z value in the 
				// translate.set(x, y, z) function as a float
				// e.g. translate.set(0.0f, 1267.7f, -2716.0f);
				// y=0.0, z=2.41
				// y=1.0, z=0.25
				// y=2.0, z=-1.9
				// y=2.7, z=-3.6
				// y=10, z=-19
				// y=100, z=-212
				// y=1000, z=-2142.5
				// y=1250, z=-2678.1
				// y=1265, z=-2710
				// y=1267.7, Z=-2716
				//
				// Lineare Regression: z = -2.14631*y + 2.46436
				// Bei nur Parametern bis einschließlich (10,-19):
				// z = -2.13825*y + 2.34609
				// Gerundet: z = -2.14*y + 2.41
				//
				// 1 y = 400 Asp.blöcke, 0.25 = 100 Asp.blöcke
				// 10 y = 4000 Aspartatblöcke
				// 1000 y = 400000 Aspartatblöcke
				// 1265 y = 499000 Aspartatblöcke
				// 1267.7 y ~ 499900 Aspartatblöcke
				translate.set(0.0f, 1267.7f, -2716.0f); // x=right left, y=front back, z=up down
				T3D.setTranslation(translate);
				viewPlatformTransform.setTransform(T3D);

				// viewPlatformTransform.setTransform(trans);

				// viewPlatform
				// 0 BasicDockableDisplayer$3,
				// 0 StackDockStation$Background, 1 JPanel, 0 BasicStackDockComponent,
				// (0,1,2,3) BasicDockableDisplayer = Scenario Tree etc

				// System.out.println(windows);
				// Component comp = getComponent(0);
				// Rectangle rect = new Rectangle(comp.getLocationOnScreen(), comp.getSize());

				// My screen is 1600 pixels wide and 900 pixels high.
				// The Display on my screen is of size 1125(width) and 643(height).
				// It starts at 464 211
				// Coordinates for mouse movement
				int xDisplayMiddle = 464 + 562;
				int yDisplayTop = 300;
				int xCoord = xDisplayMiddle;
				int yCoord = yDisplayTop;

				Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
				System.out.println("In pixels this screen's height is: " + screenSize.height
						+ " and this screen's width is: " + screenSize.width);

				// The number of notches the mouse whell is turned
				// The effect depends on your hardware.
				// So, customize this value
				int mouseWheelNotches = 15; // The number of notches

				// The shift in y direction to accomplish rotation
				int rotatingShift = 100;

				// The tube is 10^6 units long (zdim).
				// We need to drag the mouse pressed from top to bottom
				// until we reach z-value 100.
				// The view starts in the middle, therefore z-value 500,000.
				// int dragShift = 2;

				Robot robot = null;
				for (int i = 0; i < 1; i++) {
					try {
						robot = new Robot();

						// The standardization action

						// Move the mouse into the 3D Display
						robot.mouseMove(xCoord, yCoord);

						// Zoom out of the 3D Display
						// robot.mouseWheel(mouseWheelNotches);

						// Rotate 3D Display
						// robot.mousePress(InputEvent.BUTTON3_DOWN_MASK);
						// yCoord = yCoord + rotatingShift;
						robot.delay(500);
						// robot.mouseMove(xCoord, yCoord);
						// robot.mouseRelease(InputEvent.BUTTON3_DOWN_MASK);
						// robot.delay(100);
						// robot.mouseMove(xDisplayMiddle, yDisplayTop);

						/*
						 * //Drag with Right-Click to get to bacteria position
						 * robot.mousePress(InputEvent.BUTTON3_DOWN_MASK); robot.delay(1000); yCoord =
						 * yCoord + dragShift; robot.mouseMove(xCoord, yCoord); robot.delay(500);
						 * robot.mouseRelease(InputEvent.BUTTON3_DOWN_MASK); robot.delay(100);
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

	private <T extends Component> List<T> harvestMatches(Container root, Class<T> clazz) {
		List<Container> containers = new LinkedList<>();
		List<T> harvested = new ArrayList<>();

		containers.add(root);
		while (!containers.isEmpty()) {
			Container container = containers.remove(0);
			for (Component component : container.getComponents()) {
				if (clazz.isAssignableFrom(component.getClass())) {
					harvested.add((T) component);
				} else if (component instanceof Container) {
					containers.add((Container) component);
				}
			}
		}

		return Collections.unmodifiableList(harvested);
	}
}
