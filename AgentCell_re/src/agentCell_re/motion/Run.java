/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*

AgentCell is an agent-based simulation of bacterial chemotaxis.
Copyright (c) 2005 Trustees of the University of Chicago

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Authors: Thierry Emonet (emonet@uchicago.edu) and Michael J. North (north@anl.gov)

	Thierry Emonet
	920 E. 58th Street
	University of Chicago
	Chicago, IL 60637

	Michael J. North
	9700 S. Cass Avenue
	Argonne, IL 60439

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
 * @author emonet
 *
 *         Create a simple Run motion.
 *
 */
public class Run implements Motion {
	MotionStepper motionStepper;
	private double velocity = 0;
	private double rotationalDiffusion = 0;

	/**
	 * @param newMotionStepper
	 * @param newVelocity
	 * @param newRotationalDiffusion
	 */
	public Run(MotionStepper newMotionStepper, double newVelocity, double newRotationalDiffusion) {
		motionStepper = newMotionStepper;
		velocity = newVelocity;
		rotationalDiffusion = newRotationalDiffusion;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uchicago.agentcell.motion.Motion#step(double)
	 */
	public void step(double dt) {
		// TODO: Change Repast Model to Run-Model (Leon Metzger)
		// Advance position
		motionStepper.getCell().getPosition().plusMult(dt * velocity,
				motionStepper.getCell().getOrientation().viewDirection());
		// Advance orientation due to rotational diffusion if necessary
		if (rotationalDiffusion > 0) {
			// We follow:
			// Gang Zou* and Robert D. Skeely
			// Biophysical Journal Volume 85 October 2003 2147 2157
			// We use the unbiased BD numerical scheme on p. 2154:
			//
			// Get the vector of small angles using an Euler method:
			// dAngle = (dAngle0, dAngle1, dAngle2), where dAngle0 = sqrt( dt * 2 * Dr) *
			// N(0,1)
			double sqrtDt2rotationalDiffusion = Math.sqrt(dt * 2 * rotationalDiffusion);
			double dAngle0 = sqrtDt2rotationalDiffusion * RandomHelper.getNormal().nextDouble();
			double dAngle1 = sqrtDt2rotationalDiffusion * RandomHelper.getNormal().nextDouble();
			double dAngle2 = sqrtDt2rotationalDiffusion * RandomHelper.getNormal().nextDouble();

			//
			// Apply a small rotation of angle vector dAngle to the orientation
			// O = O . R
			// with : R = (1+skew(dAngle))
			// 0 -dA2 dA1
			// and: skew(dA) = dA2 0 -dA0
			// -dA1 dA0 0
			// To keep orientation orthogonal, instead of the above we do the following
			// O = O . R
			// with : R = R(i,dAnglei) . R(j,dAnglej) . R(k,dAnglek)
			// and the order : ijk = 012, 021, 102, 120, 201, 210 chosen at random
			int axesOrder = RandomHelper.nextIntFromTo(0, 5);
			motionStepper.getCell().getOrientation().rotateAroundLocalAxes(axesOrder, dAngle0, dAngle1, dAngle2);
		}
		
		// Advance position 
		motionStepper.getCell().getPosition().plusMult(dt * velocity, 
				motionStepper.getCell().getOrientation().viewDirection());

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uchicago.agentcell.motion.Motion#setMotionStepper(edu.uchicago.agentcell.
	 * motion.MotionStepper)
	 */
	public void setMotionStepper(MotionStepper motionStepper) {
		this.motionStepper = motionStepper;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uchicago.agentcell.motion.Motion#getMotionStepper()
	 */
	public MotionStepper getMotionStepper() {
		return motionStepper;
	}
}
