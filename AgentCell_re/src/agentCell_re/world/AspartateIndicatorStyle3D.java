package agentCell_re.world;

import java.awt.Color;
import java.awt.Font;

import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.BranchGroup;
import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.Shape3D;
import org.jogamp.java3d.Transform3D;
import org.jogamp.java3d.TransformGroup;
import org.jogamp.java3d.TransparencyAttributes;
import org.jogamp.java3d.loaders.Scene;
import org.jogamp.java3d.loaders.objectfile.ObjectFile;
import org.jogamp.vecmath.Color3f;
import org.jogamp.vecmath.Vector3d;

import agentCell_re.cells.ChemotacticCell;
import repast.simphony.visualization.visualization3D.AppearanceFactory;
import repast.simphony.visualization.visualization3D.ShapeFactory;
import repast.simphony.visualization.visualization3D.style.Style3D;
import repast.simphony.visualization.visualization3D.style.TaggedAppearance;
import repast.simphony.visualization.visualization3D.style.TaggedBranchGroup;
import repast.simphony.visualization.visualization3D.style.Style3D.LabelPosition;

/**
 * The 3D style class for sugar agents. The style is a red sphere.
 *
 * @author Eric Tatara
 * @author Nick Collier
 * @version
 */

public class AspartateIndicatorStyle3D implements Style3D<AspartateIndicator> {

	public TaggedBranchGroup getBranchGroup(AspartateIndicator agent, TaggedBranchGroup taggedGroup) {

		if (taggedGroup == null || taggedGroup.getTag() == null) {
			taggedGroup = getGroup(agent);

			return taggedGroup;
		}
		return null;
	}

	private TaggedBranchGroup getGroup(AspartateIndicator agent) {
		TaggedBranchGroup taggedGroup = new TaggedBranchGroup("DEFAULT");
		Shape3D box = ShapeFactory.createBox(0.015f, 0.015f, 0.3f, agent, 0, null);
		box.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
		Appearance ap = new Appearance();
		Color3f col = agent.getIndicatorColor();
		ColoringAttributes ca = new ColoringAttributes(col, ColoringAttributes.NICEST); 
		ap.setColoringAttributes(ca);
		TransparencyAttributes ta = new TransparencyAttributes();
		ta.setTransparencyMode(ta.NICEST);
		ta.setTransparency(0.8f);
		ap.setTransparencyAttributes(ta);
		box.setAppearance(ap);;
		taggedGroup.getBranchGroup().addChild(box);
		return taggedGroup;
	}

	public float[] getRotation(AspartateIndicator agent) {
		return null;
	}

	public String getLabel(AspartateIndicator agent, String currentLabel) {
		return null;
	}

	public Color getLabelColor(AspartateIndicator agent, Color currentColor) {
		return Color.YELLOW;
	}

	public Font getLabelFont(AspartateIndicator agent, Font currentFont) {
		return null;
	}

	public LabelPosition getLabelPosition(AspartateIndicator agent, LabelPosition curentPosition) {
		return LabelPosition.NORTH;
	}

	public float getLabelOffset(AspartateIndicator agent) {
		return .035f;
	}

	public float[] getScale(AspartateIndicator agent) {
		return null;
	}

	@Override
	public TaggedAppearance getAppearance(AspartateIndicator obj, TaggedAppearance appearance, Object shapeID) {
		// TODO Auto-generated method stub
		return null;
	}
}
