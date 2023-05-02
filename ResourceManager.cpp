#include <memory>
#include <filesystem>
#include <QDir>
#include "ResourceManager.h"
#include "Image.h"
#include "Tileset.h"

namespace TilesEditor
{
	void ResourceManager::populateDirectories(const QString& searchPath, const QString& rootDir)
	{
		m_searchDirectories.insert(searchPath);

		QDir dir(searchPath);

		auto list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

		for (auto& folder : list)
		{
			populateDirectories(QString("%1%2/").arg(searchPath, folder), rootDir + folder + "/");
		}
	}

	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
		for (auto resource : m_resources)
		{
			qDebug() << "Delete resource: " << resource->getName();
			delete resource;
		}
	}

	void ResourceManager::mergeSearchDirectories(const ResourceManager& source)
	{
		for (auto& searchDir : source.m_searchDirectories)
		{
			m_searchDirectories.insert(searchDir);
		}
	}

	void ResourceManager::addSearchDir(const QString& dir)
	{

		QDir absoluteDir(dir);
		m_searchDirectories.insert(absoluteDir.absolutePath() + "/");
	}


	void ResourceManager::addSearchDirRecursive(const QString& dir)
	{
		QDir absoluteDir(dir);
		populateDirectories(absoluteDir.absolutePath() + "/");

		for (auto path : m_searchDirectories)
			qDebug() << "A: " << path;
	}

	bool ResourceManager::locateFile(const QString& name, QString* outPath)
	{
		if (name != "")
		{
			for (auto& dir : m_searchDirectories)
			{
				QString fullPath = dir + name;
				qDebug() << fullPath;
				if (QFile::exists(fullPath))
				{
					*outPath = fullPath;
					return true;
				}
			}
		}

		qDebug() << "NOT FOUND";
		*outPath = "";
		return false;
	}

	bool ResourceManager::containsDirectory(const QString& dir)
	{
		return m_searchDirectories.find(dir) != m_searchDirectories.end();
	}


	void ResourceManager::addPersistantResource(Resource* resource)
	{
		resource->incrementRef();

		if(m_resources.find(resource->getName()) == m_resources.end())
			m_resources[resource->getName()] = resource;
	}

	bool ResourceManager::writeTextFile(const QString& fileName, const QString& contents)
	{
		/*
		auto fHandle = FileSystem::Instance()->OpenStream(m_rootDir + fileName, "w");
		if (fHandle != nullptr)
		{
			fHandle->Write(contents.c_str(), 1, contents.length());
			fHandle->Close();
			delete fHandle;
		}*/

		return false;
	}
	
	QString ResourceManager::readTextFile(const QString& fileName)
	{
		return "";
	}

	Resource* ResourceManager::loadResource(const QString& resourceName, ResourceType type)
	{
		auto it = m_resources.find(resourceName);
		if (it != m_resources.end())
		{
		
			auto resource = it.value();

			if (resource->getResourceType() == type)
			{
				resource->incrementRef();
				return resource;
			}
			return nullptr;
		}
		else if (m_failedResources.find(resourceName) == m_failedResources.end())
		{
			Resource* res = nullptr;


			//If they wernt loaded by the threaded method use the normal method
			QString fileName;

			if (locateFile(resourceName, &fileName))
			{
				if (type == ResourceType::RESOURCE_IMAGE)
					res = Image::load(resourceName, fileName);
				else if (type == ResourceType::RESOURCE_TILESET)
					res = Tileset::loadTileset(resourceName, fileName, *this);

				if (res == nullptr)
				{
					m_failedResources.insert(resourceName);
				}
			}

			if (res != nullptr)
			{
				res->incrementRef();
				m_resources[resourceName] = res;
			}
			return res;
		}
		return nullptr;
	}



	void ResourceManager::freeResource(Resource* resource)
	{
		if (resource == nullptr)
			return;


		if (resource->decrementRef() == 0)
		{
			auto it = m_resources.find(resource->getName());

			if (it != m_resources.end())
			{
				qDebug() << "Delete resource: " << resource->getName();
				delete resource;
				m_resources.erase(it);
			}
		}
	}



};
