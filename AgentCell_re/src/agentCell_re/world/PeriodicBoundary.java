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
 * Created on Feb 28, 2004
 *
 * 
 * 
 */
package agentCell_re.world;

import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;


/**
 * @author emonet
 *
 * 
 * 
 */
public class PeriodicBoundary extends AbstractBoundary {
    // default has period = 1
    private double period = 1;

    /**
     * @param newWorld
     * @param p0
     * @param p1
     * @param p2
     * @param n0
     * @param n1
     * @param n2
     */
    public PeriodicBoundary(IWorld newWorld, double p0, double p1, double p2,
        double n0, double n1, double n2, double newPeriod) {
        super(newWorld, p0, p1, p2, n0, n1, n2);
        type = Boundary.PERIODIC;
        period = newPeriod;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.world.Boundary#applyToPosition(edu.uchicago.agentcell.math.Vect)
     */
    public void applyToPosition(Vect p) {
        // coordinate along normal
        double x = this.distanceTo(p);

        // new coordinate along normal. If low is lower boundary, then:
        // (((x - low) % period) + period) % period + low
        // here low = -period
        double xnew = ((((x + period) % period) + period) % period) - period;

        // move point along normal
        p.plusMult(xnew - x, normal);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.world.Boundary#applyToOrientation(edu.uchicago.agentcell.math.Orientation)
     * dummy method that do nothing: periodic boudaries do not affect the orientation
     */
    public void applyToOrientation(Orientation o) {
        //a periodic boundary does not affect the orientation
    }

    /**
     * @return
     */
    public double getPeriod() {
        return period;
    }

    /**
     * @param d
     */
    public void setPeriod(double newPeriod) {
        period = newPeriod;
    }
}
