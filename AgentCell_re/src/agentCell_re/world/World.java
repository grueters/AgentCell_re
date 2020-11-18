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
package agentCell_re.world;

import java.util.ArrayList;
import java.util.Collection;

import agentCell_re.models.ChemotaxisModel;
import agentCell_re.molecules.ConcentrationField;


/**
 * @author emonet
 *
 * 
 * 
 */
public class World implements IWorld {
    private BoundaryConditions boundaryConditions;
    private Collection cells;
    private ChemotaxisModel model;

    //TODO : implement a collection of ConcentrationField and a mechanism to access the concentrations by molecularType, having UNDEFINED returned if no member in the collection can provide the wanted concentration.
    private ConcentrationField concentrationField;

    /**
     *
     */
    public World() {
        super();
        this.cells = new ArrayList();
    }

    //setters and getters
    public Collection getCells() {
        return cells;
    }

    public void setCells(Collection newCells) {
        this.cells = newCells;
    }

    public ChemotaxisModel getChemotaxisModel() {
        return model;
    }

    public void setChemotaxisModel(ChemotaxisModel chemotaxisModel) {
        this.model = chemotaxisModel;
    }

    /**
     * @return
     */
    public BoundaryConditions getBoundaryConditions() {
        return boundaryConditions;
    }

    /**
     * @param conditions
     */
    public void setBoundaryConditions(BoundaryConditions conditions) {
        boundaryConditions = conditions;
    }

    /**
     * @return
     */
    public ChemotaxisModel getModel() {
        return model;
    }

    /**
     * @param model
     */
    public void setModel(ChemotaxisModel model) {
        this.model = model;
    }

    /**
     * @return
     */
    public ConcentrationField getConcentrationField() {
        return concentrationField;
    }

    /**
     * @param field
     */
    public void setConcentrationField(ConcentrationField field) {
        concentrationField = field;
    }
}
