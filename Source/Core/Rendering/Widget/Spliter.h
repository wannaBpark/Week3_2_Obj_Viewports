#pragma once
#include "Core/Rendering/Widget/SPanel.h"

namespace ESplitterResizeMode
{
	enum Type
	{
		/** Resize the selected slot. If space is needed, then resize the next resizable slot. */
		FixedPosition,
		/** Resize the selected slot. If space is needed, then resize the last resizable slot. */
		FixedSize,
		/** Resize the selected slot by redistributing the available space with the following resizable slots. */
		Fill,
	};
}

class FLayoutGeometry;

class Spliter : public SPanel
{
public:
	/** How should a child's size be determined */
	enum ESizeRule
	{
		/** Get the DesiredSize() of the content */
		SizeToContent,
		/** Use a fraction of the parent's size */
		FractionOfParent
	};
public:

};

