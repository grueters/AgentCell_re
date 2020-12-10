package agentCell_re.cells;

import java.awt.Color;
import java.awt.Font;

import javax.media.j3d.Shape3D;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.media.j3d.TransparencyAttributes;

import agentCell_re.math.Vect;
import repast.simphony.visualization.visualization3D.AppearanceFactory;
import repast.simphony.visualization.visualization3D.ShapeFactory;
import repast.simphony.visualization.visualization3D.style.Style3D;
import repast.simphony.visualization.visualization3D.style.TaggedAppearance;
import repast.simphony.visualization.visualization3D.style.TaggedBranchGroup;

/**
 * The 3D style class for sugar agents.   
 * The style is a red sphere. 
 *
 * @author Eric Tatara
 * @author Nick Collier
 * @version 
 */

public class ChemotacticCellStyle3D implements Style3D<ChemotacticCell> {

	private static Color cellColor = new Color(0, 0, 255);

	public TaggedBranchGroup getBranchGroup(ChemotacticCell agent, TaggedBranchGroup taggedGroup) {

		if (taggedGroup == null || taggedGroup.getTag() == null) {
			taggedGroup = getGroup(agent);
			return taggedGroup;
		} 
		return null;
	}

	/**
	 * @param agent
	 * @return
	 */
	private TaggedBranchGroup getGroup(ChemotacticCell agent) {
		TaggedBranchGroup taggedGroup;
		taggedGroup = new TaggedBranchGroup("CELL");
		Shape3D shape = ShapeFactory.createCone(0.03f, 0.06f, "DEFAULT");
		shape.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
		
		Transform3D transform3d = new Transform3D();
		transform3d.rotX(Math.toRadians(270));
		TransformGroup transShape = new TransformGroup(transform3d);
		transShape.addChild(shape);
		
		taggedGroup.getBranchGroup().addChild(transShape);
		return taggedGroup;
	}

	public float[] getRotation(ChemotacticCell agent) {
		Vect v = null;
		if(agent instanceof ChemotacticCell){
			v = agent.getOrientation().viewDirection();
		}
		float[] viewDirection = new float[] {(float) v.getElement(0), (float) v.getElement(1), (float) v.getElement(2)};
		return viewDirection;
	}

	public String getLabel(ChemotacticCell agent, String currentLabel) {
		return null; 
	}

	public Color getLabelColor(ChemotacticCell agent, Color currentColor) {
		return Color.YELLOW;
	}

	public Font getLabelFont(ChemotacticCell agent, Font currentFont) {
		return null;
	}

	public LabelPosition getLabelPosition(ChemotacticCell agent, LabelPosition curentPosition) {
		return LabelPosition.NORTH;
	}

	public float getLabelOffset(ChemotacticCell agent) {
		return .035f;
	}

	public TaggedAppearance getAppearance(ChemotacticCell agent, TaggedAppearance taggedAppearance, Object shapeID) {
		if (taggedAppearance == null) {
			taggedAppearance = new TaggedAppearance();
		} 

		if (agent instanceof ChemotacticCell)
		  AppearanceFactory.setMaterialAppearance(taggedAppearance.getAppearance(), cellColor);
	
		return taggedAppearance;
	}

	public float[] getScale(ChemotacticCell agent) {
		float v = (float) agent.getVolume();
		v *= 1.0E15;
		float[] scale = {v,v,v};
		return scale;
	}
}
