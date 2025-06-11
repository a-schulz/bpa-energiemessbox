# Developer Notebook

## Docker Notes

### Common Issues
- **Issue**: `docker compose up` can fail due to wrong permissions.  
  **Solution**: Run the following command to fix permissions:
  ```bash
  sudo chown -R 1000:1000 nodered/data
  
_See also [this](https://nodered.org/docs/getting-started/docker#using-a-host-directory-for-persistence-bind-mount) guide.


## Additional information

### Mosquitto MQTT Setup
- https://github.com/sukesh-ak/setup-mosquitto-with-docker

### Drools locally
- https://hub.docker.com/r/jboss/drools-workbench-showcase
- Step by step usage of the showcase image
  - https://vishnu-chalil.medium.com/how-to-setup-drools-workbench-in-local-machine-with-docker-image-c846ca37eab2