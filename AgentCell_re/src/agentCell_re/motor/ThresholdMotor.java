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
 * Created on Mar 1, 2004
 *
 *
 * 
 */
package agentCell_re.motor;

import agentCell_re.cells.ChemotacticCell;
import agentCell_re.math.Calculus;


/**
 * @author emonet
 *
 */
public abstract class ThresholdMotor extends Motor {
    private double minCCWDuration;
    private double minCWDuration;
    private double CCWDuration;
    private double CWDuration;

    /**
     * Create a Motor (two states: CW and CCW) that switches state
     * when a threshold is crossed. CW and CCW events shorter than
     * minCWDuration and minCCWDuration respectively are ignored.
     * To keep all the events, just set minCWDuration = minCCWDuration = dt
     *
     * @param newCell
     * @param newState
     * @param newMinCWDuration
     * @param newMinCCWDuration
     */
    public ThresholdMotor(ChemotacticCell newCell, int newState,
        double newMinCWDuration, double newMinCCWDuration) {
        super(newCell, newState);

        // setting minimum duration of a state. Must be >= dt
        if (newMinCCWDuration <= 0) {
            System.err.println(
                "The minCCWDuration cannot be <= 0. It must be >= than the time step.");
            System.err.flush();
            System.exit(1);
        } else {
            minCCWDuration = newMinCCWDuration;
        }

        if (newMinCWDuration <= 0) {
            System.err.println(
                "The minCWDuration cannot be <= 0. It must be >= than the time step.");
            System.err.flush();
            System.exit(1);
        } else {
            minCWDuration = newMinCWDuration;
        }

        // making sure minCWDuration = minCCWDuration
        // TODO: change the algorithm or eliminate minCWDuration and
        // minCCWDuration for minIntervalDuration
        if (minCWDuration != minCCWDuration) {
            System.err.println(
                "INIT ERROR: minCWDuration and minCCWDuration must be equal " +
                "(until we rewrite this algorithm to handle different durations). " +
                "To see that it does not work, try the following input: " +
                "[1,1,1,0,0,1,1,1,1], with minCWDuration=0.01, " +
                "and minCCWDuration=0.02, dt=0.01. In the output the 0,0 " +
                "will be replaced by 1,0,1, making a 0 event SHORTER " +
                "than minCCWDuration=0.02.");
            System.err.flush();
            System.exit(1);
        }

        // initialize CWDuration and CCWDuration assuming that 
        // we have been in the same state for at leat the minimum durations defined above
        if (this.getState() == Motor.CW) {
            CWDuration = minCWDuration;
            CCWDuration = 0.0;
        } else {
            CWDuration = 0.0;
            CCWDuration = minCCWDuration;
        }
    }

    // advance motor state from t to t+dt
    // We assume  that the state at t lasts until t+dt
    public void step(double dt) {
        // update state duration counters 
        if (isAboveThreshold()) {
            //System.out.println("Above threshold");
            CWDuration = CWDuration + dt;
            CCWDuration = 0.0;
        } else {
            //System.out.println("Below threshold");
            CWDuration = 0.0;
            CCWDuration = CCWDuration + dt;
        }

        //System.out.println("CCWDuration = "+CCWDuration);
        //System.out.println("CWDuration    = "+CWDuration);
        //System.out.println("State  = "+this.getState());
        // Check if duration of opposite state is above min duration. If it is the case
        // we switch
        if (this.getState() == Motor.CW) {
            // we take into account Calculus.TOLERANCE for the >= tests
            if (Calculus.greaterEqualThan(CCWDuration, minCCWDuration)) {
                this.switchState();
            }
        } else {
            if (Calculus.greaterEqualThan(CWDuration, minCWDuration)) {
                this.switchState();
            }
        }
    }

    public abstract boolean isAboveThreshold();

    /**
     * @return
     */
    public double getMinCCWDuration() {
        return minCCWDuration;
    }

    /**
     * @return
     */
    public double getMinCWDuration() {
        return minCWDuration;
    }

    /**
     * @param d
     */
    public void setMinCCWDuration(double d) {
        minCCWDuration = d;
    }

    /**
     * @param d
     */
    public void setMinCWDuration(double d) {
        minCWDuration = d;
    }
    
    public double getCCWDuration() {
        return CCWDuration;
    }

    public double getCWDuration() {
        return CWDuration;
    }
}
