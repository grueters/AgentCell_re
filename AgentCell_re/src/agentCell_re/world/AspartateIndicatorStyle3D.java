package agentCell_re.world;

import java.awt.Color;
import java.awt.Font;

import org.jogamp.java3d.Appearance;
import org.jogamp.java3d.ColoringAttributes;
import org.jogamp.java3d.Shape3D;
import org.jogamp.java3d.TransparencyAttributes;
import org.jogamp.vecmath.Color3f;

import agentCell_re.models.AC_Parameters;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.parameter.Parameters;
import repast.simphony.visualization.visualization3D.ShapeFactory;
import repast.simphony.visualization.visualization3D.style.Style3D;
import repast.simphony.visualization.visualization3D.style.TaggedAppearance;
import repast.simphony.visualization.visualization3D.style.TaggedBranchGroup;

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

	AC_Parameters acParams;
	
	private TaggedBranchGroup getGroup(AspartateIndicator agent) {
		Parameters p = RunEnvironment.getInstance().getParameters();
		acParams = AC_Parameters.getInstance(p);
		
		TaggedBranchGroup taggedGroup = new TaggedBranchGroup("DEFAULT");
		float xSize = 0.015f * (float) acParams.getXdim()/5.0f;
		float ySize = 0.015f * (float) acParams.getYdim()/5.0f;
		Shape3D box = ShapeFactory.createBox(xSize, ySize, 0.3f, agent, 0, null);
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
		float v = 1f;
		if (RunEnvironment.getInstance().getCurrentSchedule().getTickCount() < 0.01) {
			v = 0.05f;
		}
		else {
			v = 1f;
		}
		float[] scale = { v, v, v };
		return scale;
	}

	@Override
	public TaggedAppearance getAppearance(AspartateIndicator obj, TaggedAppearance appearance, Object shapeID) {
		// TODO Auto-generated method stub
		return null;
	}
}
