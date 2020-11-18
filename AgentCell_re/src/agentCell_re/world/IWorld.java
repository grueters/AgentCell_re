package agentCell_re.world;

import java.util.Collection;

import agentCell_re.models.ChemotaxisModel;
import agentCell_re.molecules.ConcentrationField;

public interface IWorld {

	//setters and getters
	Collection getCells();

	void setCells(Collection newCells);

	ChemotaxisModel getChemotaxisModel();

	void setChemotaxisModel(ChemotaxisModel chemotaxisModel);

	/**
	 * @return
	 */
	BoundaryConditions getBoundaryConditions();

	/**
	 * @param conditions
	 */
	void setBoundaryConditions(BoundaryConditions conditions);

	/**
	 * @return
	 */
	ChemotaxisModel getModel();

	/**
	 * @param model
	 */
	void setModel(ChemotaxisModel model);

	/**
	 * @return
	 */
	ConcentrationField getConcentrationField();

	/**
	 * @param field
	 */
	void setConcentrationField(ConcentrationField field);

}

