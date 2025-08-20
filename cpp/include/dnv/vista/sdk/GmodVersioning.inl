/**
 * @file GmodVersioning.inl
 * @brief Inline implementations for performance-critical GmodVersioning operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodVersioning class
	//=====================================================================

	//----------------------------------------------
	// Private validation methods
	//----------------------------------------------

	inline void GmodVersioning::validateSourceAndTargetVersions( VisVersion sourceVersion, VisVersion targetVersion ) const
	{
		if ( sourceVersion == VisVersion::Unknown )
		{
			throw std::invalid_argument( "Invalid source VIS Version: Unknown" );
		}

		if ( targetVersion == VisVersion::Unknown )
		{
			throw std::invalid_argument( "Invalid target VIS Version: Unknown" );
		}

		if ( sourceVersion >= targetVersion )
		{
			throw std::invalid_argument( "Source version must be earlier than target version" );
		}
	}

	inline void GmodVersioning::validateSourceAndTargetVersionPair( VisVersion sourceVersion, VisVersion targetVersion ) const
	{
		if ( sourceVersion >= targetVersion )
		{
			throw std::invalid_argument( "Source version must be less than target version" );
		}

		if ( static_cast<int>( targetVersion ) - static_cast<int>( sourceVersion ) != 100 )
		{
			throw std::invalid_argument( "Target version must be exactly one version higher than source version" );
		}
	}

	//----------------------------------------------
	// Private helper methods
	//----------------------------------------------

	inline bool GmodVersioning::tryGetVersioningNode( VisVersion visVersion, const GmodVersioningNode*& versioningNode ) const
	{
		versioningNode = m_versioningsMap.tryGetValue( visVersion );

		return versioningNode != nullptr;
	}

	//----------------------------------------------
	// Private static utility methods
	//----------------------------------------------

	inline GmodVersioning::ConversionType GmodVersioning::parseConversionType( std::string_view type )
	{
		if ( type == "changeCode" )
		{
			return ConversionType::ChangeCode;
		}
		if ( type == "move" )
		{
			return ConversionType::Move;
		}
		if ( type == "merge" )
		{
			return ConversionType::Merge;
		}
		if ( type == "assignmentChange" )
		{
			return ConversionType::AssignmentChange;
		}
		if ( type == "assignmentDelete" )
		{
			return ConversionType::AssignmentDelete;
		}

		throw std::invalid_argument( "Invalid conversion type: " + std::string{ type } );
	}

	//----------------------------------------------
	// GmodVersioning::GmodVersioningNode class
	//----------------------------------------------

	//----------------------------
	// Accessors
	//----------------------------

	inline VisVersion GmodVersioning::GmodVersioningNode::visVersion() const
	{
		return m_visVersion;
	}

	inline bool GmodVersioning::GmodVersioningNode::tryGetCodeChanges(
		std::string_view code,
		const GmodNodeConversion*& nodeChanges ) const
	{
		auto it = m_versioningNodeChanges.find( code );
		if ( it != m_versioningNodeChanges.end() )
		{
			nodeChanges = &it->second;

			return true;
		}

		return false;
	}
}
