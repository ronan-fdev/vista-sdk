/**
 * @file LocalIdItems.cpp
 * @brief Implementation of the LocalIdItems class.
 */
#include "pch.h"

#include "dnv/vista/sdk/LocalIdItems.h"

#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdItems class
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	LocalIdItems::LocalIdItems( GmodPath&& primaryItem, std::optional<GmodPath>&& secondaryItem )
		: m_primaryItem{ std::move( primaryItem ) },
		  m_secondaryItem{ std::move( secondaryItem ) }
	{
	}

	LocalIdItems::LocalIdItems( LocalIdItems&& other, GmodPath&& newPrimaryItem )
		: m_primaryItem{ std::move( newPrimaryItem ) },
		  m_secondaryItem{ std::move( other.m_secondaryItem ) }
	{
	}

	LocalIdItems::LocalIdItems( LocalIdItems&& other, std::optional<GmodPath>&& newSecondaryItem )
		: m_primaryItem{ std::move( other.m_primaryItem ) },
		  m_secondaryItem{ std::move( newSecondaryItem ) }
	{
	}
}
