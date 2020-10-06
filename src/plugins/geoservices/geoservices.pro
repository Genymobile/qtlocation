TEMPLATE = subdirs

QT_FOR_CONFIG += location-private  # pulls in the features defined in configure.json

qtConfig(geoservices_mapbox): SUBDIRS += mapbox
qtConfig(geoservices_osm): SUBDIRS += osm
