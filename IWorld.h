#ifndef IWORLDH
#define IWORLDH

#include <QSet>
#include "Rectangle.h"
#include "ResourceManager.h"

namespace TilesEditor
{
	class Level;
	class AbstractLevelEntity;
	class IWorld
	{
	public:

		virtual QSet<Level*> getLevelsInRect(const IRectangle& rect) = 0;
		virtual Level* getLevelAt(double x, double y) = 0;
		virtual ResourceManager& getResourceManager() = 0;
		virtual AbstractLevelEntity* getEntityAt(double x, double y) = 0;
		virtual bool tryGetTileAt(double x, double y, int* outTile) = 0;
		virtual void deleteEntity(AbstractLevelEntity* entity) = 0;
		virtual bool containsLevel(const QString& levelName)const = 0;
		virtual void centerLevel(const QString& levelName) = 0;
		virtual void setModified(Level* level) = 0;
		virtual void updateMovedEntity(AbstractLevelEntity* entity) = 0;
		virtual QList<Level*> getModifiedLevels() = 0;
	};
};
#endif
