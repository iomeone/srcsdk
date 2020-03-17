//====== Copyright � 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "movieobjects/dmeshader.h"
#include "datamodel/dmelementfactoryhelper.h"
#include "movieobjects_interfaces.h"

#include "materialsystem/IShader.h"
#include "materialsystem/imaterialsystem.h"
#include "tier1/KeyValues.h"

//-----------------------------------------------------------------------------
// Expose this class to the scene database 
//-----------------------------------------------------------------------------
IMPLEMENT_ELEMENT_FACTORY( DmeShader, CDmeShader );


//-----------------------------------------------------------------------------
// Constructor, destructor
//-----------------------------------------------------------------------------
void CDmeShader::OnConstruction()
{
	m_ShaderName.Init( this, "shaderName" );

	m_ShaderName = "wireframe";
	m_pShader = NULL;
}

void CDmeShader::OnDestruction()
{
}


//-----------------------------------------------------------------------------
// Shader name access
//-----------------------------------------------------------------------------
void CDmeShader::SetShaderName( const char *pShaderName )
{
	m_ShaderName = pShaderName;
}

const char *CDmeShader::GetShaderName() const
{
	return m_ShaderName.Get();
}


//-----------------------------------------------------------------------------
// Finds a shader
//-----------------------------------------------------------------------------
IShader *CDmeShader::FindShader()
{
	int nCount = g_pMaterialSystem->ShaderCount();
	IShader **ppShaderList = (IShader**)stackalloc( nCount * sizeof(IShader*) );
	g_pMaterialSystem->GetShaders( 0, nCount, ppShaderList );
	for ( int i = 0; i < nCount; ++i )
	{
		if ( !Q_stricmp( m_ShaderName.Get(), ppShaderList[i]->GetName() ) )
			return ppShaderList[i];
	}
	return NULL;
}


//-----------------------------------------------------------------------------
// Remove all shader parameters that don't exist in the new shader
//-----------------------------------------------------------------------------
void CDmeShader::RemoveUnusedShaderParams( IShader *pShader )
{
	CDmAttribute* pAttribute = FirstAttribute();
	CDmAttribute* pNextAttribute = NULL;
	for ( ; pAttribute; pAttribute = pNextAttribute )
	{
		pNextAttribute = pAttribute->NextAttribute();

		// Don't remove name, type, or id
		if ( pAttribute->IsFlagSet( FATTRIB_STANDARD ) )
			continue;

		const char *pShaderParam = pAttribute->GetName();
		int nCount = pShader->GetNumParams();
		int i;
		for ( i = 0; i < nCount; ++i )
		{
			if ( !Q_stricmp( pShaderParam, pShader->GetParamName( i ) ) )
				break;
		}

		// No match? Remove it!
		if ( i == nCount )
		{
			RemoveAttributeByPtr( pAttribute );
		}
	}
}


//-----------------------------------------------------------------------------
// Add attribute for shader parameter
//-----------------------------------------------------------------------------
CDmAttribute* CDmeShader::AddAttributeForShaderParameter( IShader *pShader, int nIndex )
{
	ShaderParamType_t paramType = pShader->GetParamType( nIndex );
	const char *pParamName = pShader->GetParamName( nIndex );

	CDmAttribute *pAttribute = NULL;
	switch ( paramType )
	{
	case SHADER_PARAM_TYPE_INTEGER:
		pAttribute = AddAttribute( pParamName, AT_INT );
		break;

	case SHADER_PARAM_TYPE_BOOL:
		pAttribute = AddAttribute( pParamName, AT_BOOL );
		break;

 	case SHADER_PARAM_TYPE_FLOAT:
		pAttribute = AddAttribute( pParamName, AT_FLOAT );
		break;

	case SHADER_PARAM_TYPE_STRING:
		pAttribute = AddAttribute( pParamName, AT_STRING );
		break;

	case SHADER_PARAM_TYPE_COLOR:
		pAttribute = AddAttribute( pParamName, AT_COLOR );
		break;

	case SHADER_PARAM_TYPE_VEC2:
		pAttribute = AddAttribute( pParamName, AT_VECTOR2 );
		break;

	case SHADER_PARAM_TYPE_VEC3:
		pAttribute = AddAttribute( pParamName, AT_VECTOR3 );
		break;

	case SHADER_PARAM_TYPE_VEC4:
		pAttribute = AddAttribute( pParamName, AT_VECTOR4 );
		break;

	case SHADER_PARAM_TYPE_FOURCC:
		Assert( 0 );
		break;

	case SHADER_PARAM_TYPE_MATRIX:
		pAttribute = AddAttribute( pParamName, AT_VMATRIX );
		break;

	case SHADER_PARAM_TYPE_TEXTURE:
		pAttribute = AddAttribute( pParamName, AT_ELEMENT );
		break;

	case SHADER_PARAM_TYPE_MATERIAL:
		pAttribute = AddAttribute( pParamName, AT_ELEMENT );
		break;

	default:
		break;
	}
	return pAttribute;
}


//-----------------------------------------------------------------------------
// Add all shader parameters that don't currently exist
//-----------------------------------------------------------------------------
void CDmeShader::AddNewShaderParams( IShader *pShader )
{
	int nCount = pShader->GetNumParams();
	int i;
	for ( i = 0; i < nCount; ++i )
	{
		const char *pParamName = pShader->GetParamName( i );

		CDmAttribute* pAttribute = NULL;
		for ( pAttribute = FirstAttribute(); pAttribute; pAttribute = pAttribute->NextAttribute() )
		{
			// Don't remove name, type, or id
			if ( pAttribute->IsFlagSet( FATTRIB_STANDARD ) )
				continue;

			const char *pAttributeName = pAttribute->GetName();
			if ( !Q_stricmp( pAttributeName, pParamName ) )
				break;
		}

		// No match? Add it!
		if ( pAttribute != NULL )
			continue;

		pAttribute = AddAttributeForShaderParameter( pShader, i );
		if ( pAttribute )
		{
			const char *pDefault = pShader->GetParamDefault( i );

			pAttribute->SetValueFromString( pDefault );
		}
	}
}


//-----------------------------------------------------------------------------
// resolve
//-----------------------------------------------------------------------------
void CDmeShader::Resolve()
{
	if ( !m_ShaderName.IsDirty() || !g_pMaterialSystem )
		return;

	// First, find the shader
	IShader *pShader = FindShader();

	// Remove all shader parameters that don't exist in the new shader
	RemoveUnusedShaderParams( pShader );

	// Add all shader parameters that don't currently exist
	AddNewShaderParams( pShader );
}


//-----------------------------------------------------------------------------
// Returns a procedural material to be associated with this shader
//-----------------------------------------------------------------------------
IMaterial *CDmeShader::CreateMaterial( IShader *pShader, const char *pMaterialName )
{
	KeyValues *pVMTKeyValues = new KeyValues( GetShaderName() );

	CDmAttribute* pAttribute = FirstAttribute();
	CDmAttribute* pNextAttribute = NULL;
	for ( ; pAttribute; pAttribute = pNextAttribute )
	{
		pNextAttribute = pAttribute->NextAttribute();

		// Don't remove name, type, or id
		if ( pAttribute->IsFlagSet( FATTRIB_STANDARD ) )
			continue;

		const char *pShaderParam = pAttribute->GetName();
		int nCount = pShader->GetNumParams();
		int i;
		for ( i = 0; i < nCount; ++i )
		{
			if ( !Q_stricmp( pShaderParam, pShader->GetParamName( i ) ) )
				break;
		}

		// No match? Remove it!
		if ( i == nCount )
		{
			RemoveAttributeByPtr( pAttribute );
		}
	}

	pVMTKeyValues->SetInt( "$model", 1 );
	pVMTKeyValues->SetFloat( "$decalscale", 0.05f );
	pVMTKeyValues->SetString( "$basetexture", "error" );
	return g_pMaterialSystem->CreateMaterial( pMaterialName, pVMTKeyValues );
}
