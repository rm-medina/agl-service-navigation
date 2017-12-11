// Copyright 2017 AW SOFTWARE CO.,LTD
// Copyright 2017 AISIN AW CO.,LTD

#include "genivi/navicore.h"
#include "genivi/genivi-navicore-constants.h"
#include "genivi_request.h"
#include <stdio.h>
#include <exception>
#include <dbus-c++-1/dbus-c++/dbus.h>

/**
 *  @brief Destructor
 */
GeniviRequest::~GeniviRequest()
{
	delete (Navicore*)navicore_;
	navicore_ = NULL;
}

/**
 *  @brief  DBus session creation
 */
void GeniviRequest::CreateDBusSession( )
{
	try
	{
		static DBus::BusDispatcher dispatcher;
		DBus::default_dispatcher = &dispatcher;
		DBus::Connection conn = DBus::Connection::SessionBus();

		navicore_ = new Navicore(conn, "/org/genivi/navicore", "org.agl.naviapi");
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}
}

/**
 *  @brief	  Check connection status
 *  @return	 Connection status
 */
bool GeniviRequest::CheckSession()
{
	if(this->navicore_ == NULL)
	{
		this->CreateDBusSession();
	}

	try
	{
		// get connection status
		DBus::Connection conn = ((Navicore*)navicore_)->conn();
		bool isConnect = conn.connected();

		// When it is not connected
		if(!isConnect)
		{
			fprintf(stderr, "Service has no session.\n");
		}

		return isConnect;
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
		return false;
	}
}

/**
 *  @brief	  GeniviAPI GetPositionを呼び出し、情報を取得する
 *  @param[in]  valuesToReturn Key array of information to retrieve using API
 *  @return	 Geniviから取得した情報のキーと値のmap情報
 */
std::map< int32_t, double > GeniviRequest::NavicoreGetPosition( const std::vector< int32_t >& valuesToReturn )
{
	std::map< int32_t, double > ret;

	if( !CheckSession() )
	{
		return ret;
	}

	try
	{
		std::map< int32_t, ::DBus::Struct< uint8_t, ::DBus::Variant > >::iterator it;
		std::map< int32_t, ::DBus::Struct< uint8_t, ::DBus::Variant > > PosList =
			((Navicore*)navicore_)->GetPosition(valuesToReturn);
		for (it = PosList.begin(); it != PosList.end(); it++)
		{
			if (it->first == NAVICORE_LATITUDE || it->second._1 == NAVICORE_LATITUDE)
			{
				ret[it->first] = it->second._2.reader().get_double();
			}
			else if (it->first == NAVICORE_LONGITUDE || it->second._1 == NAVICORE_LONGITUDE)
			{
				ret[it->first] = it->second._2.reader().get_double();
			}
			else if (it->first == NAVICORE_HEADING || it->second._1 == NAVICORE_HEADING)
			{
				ret[it->first] = it->second._2.reader().get_uint32();
			}
	#if 0 // 未サポート
			else if (it->first == NAVICORE_TIMESTAMP || it->second._1 == NAVICORE_TIMESTAMP)
			{
				ret[it->first] = it->second._2.reader().get_uint32();
			}
			else if (it->first == NAVICORE_SPEED || it->second._1 == NAVICORE_SPEED)
			{
				ret[it->first] = it->second._2.reader().get_int32();
			}
	#endif
			else if (it->first == NAVICORE_SIMULATION_MODE || it->second._1 == NAVICORE_SIMULATION_MODE)
			{
				ret[it->first] = it->second._2.reader().get_bool();
			}
		}
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}

	return ret;
}


/**
 *  @brief  GeniviAPI GetPositionを呼び出し、情報を取得する
 *  @param  なし
 *  @return Geniviから取得したルートハンドル
 */
std::vector< uint32_t > GeniviRequest::NavicoreGetAllRoutes( void )
{
	if( !CheckSession() )
	{
		std::vector< uint32_t > no_route;
		return no_route;
	}

	std::vector< uint32_t > allRoutes;
	try
	{
		allRoutes = ((Navicore*)navicore_)->GetAllRoutes();
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}

	return allRoutes;
}


/**
 *  @brief	  GeniviAPI GetPositionを呼び出し、情報を取得する
 *  @param[in]  sessionHandle セッションハンドル
 *  @return	 Geniviから取得したルートハンドル
 */
uint32_t GeniviRequest::NavicoreCreateRoute( const uint32_t& sessionHandle )
{
	if( !CheckSession() )
	{
		return 0;
	}

	uint32_t routeHandle = 0;
	try
	{
		routeHandle = ((Navicore*)navicore_)->CreateRoute(sessionHandle);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}

	return routeHandle;
}


/**
 *  @brief	  GeniviAPI PauseSimulationを呼び出す
 *  @param[in]  sessionHandle セッションハンドル
 *  @return	 なし
 */
void GeniviRequest::NavicorePauseSimulation( const uint32_t& sessionHandle )
{
	if( !CheckSession() )
	{
		return;
	}

	try
	{
		((Navicore*)navicore_)->PauseSimulation(sessionHandle);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}
}


/**
 *  @brief	  GeniviAPI SetSimulationModeを呼び出す
 *  @param[in]  sessionHandle セッションハンドル
 *  @param[in]  activate シミュレーションモード有効・無効
 *  @return	 なし
 */
void GeniviRequest::NavicoreSetSimulationMode( const uint32_t& sessionHandle, const bool& activate )
{
	if( !CheckSession() )
	{
		return;
	}

	try
	{
		((Navicore*)navicore_)->SetSimulationMode(sessionHandle, activate);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}
}


/**
 *  @brief	  GeniviAPI SetSimulationModeを呼び出す
 *  @param[in]  routeHandle	 ルートハンドル
 *  @return	 なし
 */
void GeniviRequest::NavicoreCancelRouteCalculation( const uint32_t& sessionHandle, const uint32_t& routeHandle )
{
	if( !CheckSession() )
	{
		return;
	}

	try
	{
		((Navicore*)navicore_)->CancelRouteCalculation(sessionHandle, routeHandle);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}
}


/**
 *  @brief	  GeniviAPI SetWaypointsを呼び出す
 *  @param[in]  sessionHandle セッションハンドル
 *  @param[in]  routeHandle ルートハンドル
 *  @param[in]  startFromCurrentPosition 自車位置からルートを引くか否か
 *  @param[in]  waypointsList 目的地座標
 *  @return	 なし
 */
void GeniviRequest::NavicoreSetWaypoints( const uint32_t& sessionHandle, const uint32_t& routeHandle,
					const bool& startFromCurrentPosition, const std::vector<Waypoint>& waypointsList )
{
	if( !CheckSession() )
	{
		return;
	}

	std::vector<Waypoint>::const_iterator it;
	std::vector< std::map< int32_t, ::DBus::Struct< uint8_t, ::DBus::Variant > > > wpl;

	fprintf(stdout, "session: %d, route: %d, startFromCurrentPosition: %d\n",
		sessionHandle, routeHandle, startFromCurrentPosition);

	for (it = waypointsList.begin(); it != waypointsList.end(); it++)
	{
		std::map< int32_t, ::DBus::Struct< uint8_t, ::DBus::Variant > > Point;
		::DBus::Struct< uint8_t, ::DBus::Variant > VarLat, VarLon;

		VarLat._1 = NAVICORE_LATITUDE;
		VarLat._2.writer().append_double(std::get<0>(*it));
		fprintf(stdout, "VarLat._1 : %x, VarLat._2 : %lf\n", VarLat._1, VarLat._2.reader().get_double());

		VarLon._1 = NAVICORE_LONGITUDE;
		VarLon._2.writer().append_double(std::get<1>(*it));
		fprintf(stdout, "VarLon._1 : %x, VarLon._2 : %lf\n", VarLon._1, VarLon._2.reader().get_double());

		Point[NAVICORE_LATITUDE] = VarLat;
		Point[NAVICORE_LONGITUDE] = VarLon;

		wpl.push_back(Point);
	}

	try
	{
		((Navicore*)navicore_)->SetWaypoints(sessionHandle, routeHandle, startFromCurrentPosition, wpl);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}
}


/**
 *  @brief	  GeniviAPI CalculateRouteを呼び出す
 *  @param[in]  sessionHandle セッションハンドル
 *  @param[in]  routeHandle ルートハンドル
 *  @return	 なし
 */
void GeniviRequest::NavicoreCalculateRoute( const uint32_t& sessionHandle, const uint32_t& routeHandle )
{
	if( !CheckSession() )
	{
		return;
	}

	try
	{
		((Navicore*)navicore_)->CalculateRoute(sessionHandle, routeHandle);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}
}


/**
 *  @brief  GeniviAPI CalculateRouteを呼び出す
 *  @param  なし
 *  @return Geniviから取得した情報のキーと値のmap情報
 */
std::map<uint32_t, std::string> GeniviRequest::NavicoreGetAllSessions()
{
	std::map<uint32_t, std::string> ret;

	if( !CheckSession() )
	{
		return ret;
	}

	std::vector< ::DBus::Struct< uint32_t, std::string > > ncAllSessions;
	std::vector< ::DBus::Struct< uint32_t, std::string > >::iterator it;

	try
	{
		ncAllSessions = ((Navicore*)navicore_)->GetAllSessions();
		for (it = ncAllSessions.begin(); it != ncAllSessions.end(); it++)
		{
			ret[it->_1] = it->_2;
		}
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "Error:%s\n", e.what());
	}

	return ret;
}

