/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Feb 24, 2004
 *
 * 
 * 
 */
package agentCell_re.cells;

import java.io.File;

import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import agentCell_re.motion.MotionStepper;
import agentCell_re.util.general.PathInterface;
import agentCell_re.world.IWorld;


/**
 * @author emonet
 *
 * 
 * 
 */
public abstract class Cell implements PathInterface{
    private static int lastID = 0;
    private IWorld world;
    private Vect position;
    private Orientation orientation;
    private MotionStepper motionStepper;
    private double volume;
    private int identifier = Cell.lastID++;
    private String path;

    /**
     * Base abstract class for cells
     *
     * @param newWorld
     * @param newPosition
     * @param newOrientation
     * @param newVolume
     */
    public Cell(IWorld newWorld, Vect newPosition, Orientation newOrientation,
        double newVolume) {
        this.world = newWorld;
        this.position = newPosition;
        this.orientation = newOrientation;
        this.volume = newVolume;
    }

    //	Declare the major time advancer.
    public abstract void step();

    /**
     * @return
     */
    public int getIdentifier() {
        return identifier;
    }

    /**
     * @return
     */
    public MotionStepper getMotionStepper() {
        return motionStepper;
    }

    /**
     * @return
     */
    public Orientation getOrientation() {
        return orientation;
    }

    /**
     * @return
     */
    public Vect getPosition() {
        return position;
    }

    /**
     * @param stepper
     */
    public void setMotionStepper(MotionStepper stepper) {
        motionStepper = stepper;
    }

    /**
     * @param orientation
     */
    public void setOrientation(Orientation orientation) {
        this.orientation = orientation;
    }

    /**
     * @param vect3
     */
    public void setPosition(Vect3 vect3) {
        position = vect3;
    }

    /**
     * @return
     */
    public IWorld getWorld() {
        return world;
    }

    /**
     * @param world
     */
    public void setWorld(IWorld world) {
        this.world = world;
    }

    /**
     * @param vect
     */
    public void setPosition(Vect vect) {
        position = vect;
    }

    /**
     * @return
     */
    public double getVolume() {
        return volume;
    }

    /**
     * @param d
     */
    public void setVolume(double d) {
        volume = d;
    }
	
	public void setPath(String parent, String currentLoc) {
		this.path = new File(parent, currentLoc).toString();
	}
	
	public void setPath(String path) {
		this.path = path;
	}
	
	public String getPath() {
		return path;
	}

	public String getPathDir() {
		return path + PathInterface.fileSeparator;
	}

	
}
