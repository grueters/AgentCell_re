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
 *
 */
public class TumbleGammaDistribution implements Motion {
    private MotionStepper motionStepper;
    private double shape;
    private double scale;
    private double location;

    /**
     * Create a tumble motion with tumble angles drawn from a Gamma Distribution
     * (inspired from Fig. 3, p. 501 from Berg and Brown, Nature, 239, 500 (1972))
     * The tumble consists of a random rotation around the local
     * direction of motion (axis=2), followed by a rotation of angle alpha
     * around the local y axis (axis=1). alpha is drawn from a gamma
     * distribution with PDF: p(x) = exp(-(x-location)/scale) *
     * ((x-location)/scale)^(shape-1) / (scale gamma(shape))
     *
     * With shape = 4 scale = 18.32045567939674 location = -4.606176605609249 it
     * fits the distribution plotted in Fig.3, p.501 of Berg & Brown (1972)
     *
     * Note: We do not take into acount the length of the tumbling interval.
     *
     * @param newMotionStepper
     * @param newShape
     * @param newScale
     * @param newLocation
     */
    public TumbleGammaDistribution(MotionStepper newMotionStepper,
        double newShape, double newScale, double newLocation) {
        motionStepper = newMotionStepper;
        this.shape = newShape;
        this.scale = newScale;
        this.location = newLocation;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.Motion#step(double)
     */
    public void step(double dt) {
        //random rotation around direction of motion
        motionStepper.getCell().getOrientation().rotateAroundLocalAxis(2,
            RandomHelper.nextDoubleFromTo(0, 2 * Math.PI));

        //generate an angle from the gamma distribution.
        //We drop the angles bigger than 180
        double alpha;

        do {
            // Notice that the two parameters taken by Random.gamma.nextDouble() are
            // the shape parameter a and lambda = 1/b, the inverse of the scale parameter 
            // TODO check if nextDouble returns standard gamma distribution or the correctly parametrized one 
        	alpha = RandomHelper.getGamma().nextDouble(this.shape, 1 / this.scale) +
                this.location;
        } while (alpha > 180);

        alpha = alpha / 180 * Math.PI;

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
