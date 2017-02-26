/**
 * Created by Sergey on 19.02.2017.
 */
var storage =
{
    //
    get kernel_uuid()
    {
        return storage["FKernelUuid"];
    },
    set kernel_uuid(aVal)
    {
        if (storage.hasOwnProperty("FKernelUuid") && aVal !== storage.FKernelUuid)
        {
            console.error("Invalid uuid "+aVal+" lastest "+storage.FKernelUuid);
            alert("The page will be reloaded as the server has been reloaded!");
            location.reload(true);
        }
        storage["FKernelUuid"]=aVal;
    }
};