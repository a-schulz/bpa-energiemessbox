# Developer Notebook

## Docker Notes

### Common Issues
- **Issue**: `docker compose up` can fail due to wrong permissions.  
  **Solution**: Run the following command to fix permissions:
  ```bash
  sudo chown -R 1000:1000 nodered/data
  
_See also [this](https://nodered.org/docs/getting-started/docker#using-a-host-directory-for-persistence-bind-mount) guide.