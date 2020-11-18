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
package agentCell_re.motion;

import repast.simphony.random.RandomHelper;

/**
 * @author Thierry Emonet
 *
 * Create a simple tumble motion.
 * It consist of a random rotation around the local direction of motion (axis=2),
 * followed by a rotation of angle alpha around the local y axis (axis=1).
 * alpha is drawn from the distribution:  p(alpha) = cos(alpha/2)/2,  0 &le; alpha &le; Pi
 * We do not take into acount the length of the tumbling interval.
 */
public class TumbleCosinusDistribution implements Motion {
    private MotionStepper motionStepper;

    public TumbleCosinusDistribution(MotionStepper newMotionStepper) {
        motionStepper = newMotionStepper;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.Motion#step(double)
     */
    public void step(double dt) {
        //random rotation around direction of motion
        motionStepper.getCell().getOrientation().rotateAroundLocalAxis(2,
            RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));
        //generate an angle from the distribution: p(alpha) = cos(alpha/2)/2,  0 &le; alpha &le; Pi		
        double alpha = 2 * Math.asin(RandomHelper.nextDoubleFromTo(0, 1));

        //rotate by alpha around local y-axis
        motionStepper.getCell().getOrientation().rotateAroundLocalAxis(1, alpha);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.Motion#setMotionStepper(edu.uchicago.agentcell.motion.MotionStepper)
     */
    public void setMotionStepper(MotionStepper motionStepper) {
        this.motionStepper = motionStepper;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.Motion#getMotionStepper()
     */
    public MotionStepper getMotionStepper() {
        return motionStepper;
    }
}
