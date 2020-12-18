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

import agentCell_re.flagella.Flagella;
import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import agentCell_re.molecules.Copynumber;
import agentCell_re.motor.Motor;
import agentCell_re.networks.Network;
import agentCell_re.receptors.Receptors;
import agentCell_re.util.general.PathInterface;
import agentCell_re.util.hdf.ChemotaxisRecorder;
import agentCell_re.world.IWorld;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.space.continuous.ContinuousSpace;



/**
 * @author emonet
 *
 * 
 * 
 */
public class ChemotacticCell extends Cell implements PathInterface{
    private Motor motor;
    private Flagella flagella;
    private Receptors receptors;
    private Network chemotaxisNetwork;
    private Copynumber cheYp;
    private double lastTime = 0.0;
    
    // Let Cell manage it's FileLocation and HDFLogger
    private String path;    
    private ChemotaxisRecorder hdfLogger = null;
    private ContinuousSpace<Object> space3d;
    
    
    /**
     * Duration of the equilibration period after which the 
     * motor are switched on and the cell is let loose. Has no
     * effect if set to zero. 
     */
    private double equilibrationTime=0.0;
    private Vect initialPosition;
    
    /**
     * Create a chemotactic cell. It has Motor, Flagella, Receptors, Network and
     * cheyp Copynumber
     *
     * @param newWorld
     * @param newPosition
     * @param newOrientation
     * @param newCheYp
     * @param newVolume
     */
    public ChemotacticCell(ContinuousSpace<Object> space3d, IWorld newWorld, Vect newPosition,
        Orientation newOrientation, Copynumber newCheYp, double newVolume) {
        super(newWorld, newPosition, newOrientation, newVolume);
        this.space3d = space3d;
        this.cheYp = newCheYp;
        this.initialPosition = new Vect3();
        this.initialPosition.copy(newPosition);
    }

	//	Declare the major time advancer.
    @ScheduledMethod(start = 0, interval = 0.01, priority = 1)
    public void step() {
    	double dt = this.getWorld().getModel().getAcParams().getDT_s();
    	double newTime = this.getWorld().getModel().getSchedule().getTickCount();
    	// randomize orientation
		/*
		 * if(Math.abs(newTime - dt) < 1.0E-6) { this.getOrientation().randomize(); }
		 */
		 
    	
		
        // All routines called here should use do a FORWARD step of dt in time. 
        // The integration scheme should be FORWARD: f(t+dt) = f(t) + dt * df(t)
        // Note: t = lastTime. 
        //           t+ dt =  newTime = lastTime + dt
//System.out.println("*****************  ChemotacticCell " + dt + " starts step at t = " + lastTime);
        // Advance network using state of the network at t = lastTime 
        // We must do that before motion because the cell has to pick up world concentrations at 
        // its position at time t=lastTime
        if (chemotaxisNetwork != null) {
            chemotaxisNetwork.step(dt); //stochsim step
        }

        // Advance motion using position and orientation AND flagella.state at t=lastTime
        // We must do that before the motor and flagella steps because we need to use the motor state 
        // at time t=lastTime
        // Check first if MotionStepper is defined         
        if (this.getMotionStepper() != null) {
            this.getMotionStepper().step(dt);       
        }   

        // During the equilibration the cell is kept at its initial position. 
        // Usefull to get a distribution of initial conditions.
        if (newTime < this.equilibrationTime){
            this.getPosition().copy(this.initialPosition);
        }
        
        // Update the state of the motor using the state of the network at t=newTime
//System.out.println(" ****** Motor step starts in state = " + motor.getState());
        if (motor != null) {
            motor.step(dt);
        }

//System.out.println(" ****** Motor step ends  in state = " + motor.getState());
        // Advance flagella from t=lastTime to t=newTime, using the state of the motors
        // at t = newTime
        if (flagella != null) {
            flagella.step(dt);
        }
        System.out.println(
        	"Cell-ID: " + this.getIdentifier()
        	+ " | Position is: " + this.getPosition()
        	+ " | Direction = " + this.getOrientation().viewDirection()
        	+  " |\n seconds: " 
        	+ RunEnvironment.getInstance().getCurrentSchedule().getTickCount() 
        	+ " | FlagellaState(0=bundled=run,1=apart=tumble,-1=invalid): " 
        	+ this.flagella.getState() 
        	+ "\n | CheYp-Level: " + this.getCheYp().getLevel() 
        	+ " | Motor State(CCW=0, CW=1): " + this.getMotor().getState());
        space3d.moveTo(this, this.getPosition().getElement(0), this.getPosition().getElement(1), this.getPosition().getElement(2));
       lastTime = newTime;
       
//System.out.println(" *****************  ChemotacticCell " + identifier + " ends step at t = " + newTime);
    }

    /**
     * @return
     */
    public Motor getMotor() {
        return motor;
    }

    /**
     * @param motor
     */
    public void setMotor(Motor motor) {
        this.motor = motor;
    }

    /**
     * @return
     */
    public Copynumber getCheYp() {
    	return cheYp;
    }

    /**
     * @param chemical
     */
    public void setCheYp(Copynumber newCopynumber) {
        cheYp = newCopynumber;
    }

    /**
     * @return
     */
    public Receptors getReceptors() {
        return receptors;
    }

    /**
     * @param receptors
     */
    public void setReceptors(Receptors receptors) {
        this.receptors = receptors;
    }

    /**
     * @return
     */
    public Network getChemotaxisNetwork() {
        return chemotaxisNetwork;
    }

    /**
     * @param network
     */
    public void setChemotaxisNetwork(Network network) {
        chemotaxisNetwork = network;
    }

    /**
     * @return Returns the flagella.
     */
    public Flagella getFlagella() {
        return flagella;
    }

    /**
     * @param flagella The flagella to set.
     */
    public void setFlagella(Flagella flagella) {
        this.flagella = flagella;
    }

    /**
     * @return Returns the equilibrationTime.
     */
    public double getEquilibrationTime() {
        return equilibrationTime;
    }

    /**
     * @param equilibrationTime The equilibrationTime to set.
     */
    public void setEquilibrationTime(double equilibrationTime) {
        this.equilibrationTime = equilibrationTime;
        System.out.println("The cell "+this.getIdentifier() +
                    " will equilibrate for "+this.equilibrationTime+" sec");
    }

	public ChemotaxisRecorder getHdfLogger() {
		return hdfLogger;
	}

	public void setHdfLogger(ChemotaxisRecorder hdfLogger) {
		this.hdfLogger = hdfLogger;
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
	
	public double getXPosition() {
		return this.getPosition().getElement(0);
	}
	
	public double getYPosition() {
		return this.getPosition().getElement(1);
	}
	
	public double getZPosition() {
		return this.getPosition().getElement(2);
	}
}
