# Basic Architecture

## Abstract

The idea is to have a decoupled framework that allows the user to sync using free hosting solutions, empower developers to easily implement clients and servers that can be mixed/matched, and empower server admins to deploy their own servers.

## Goals

### Features
- Program is portable.
- Addons are portable.
- Addons can extend/modify/reimplement nearly all functionality.
- Can be self-hosted.
- All user data can be versioned.
- Certain kinds of user data can be unversioned and synced via different mechanisms.
- Can import from and export to major SRS solutions such as Anki and Mnemosyne.
- Can be installed or run self-contained (e.g. flash drive).
- Can be used for research purposes.
- Unicode support
- [Localization support](https://msdn.microsoft.com/en-us/library/aa241727(v=vs.60).aspx)
- Support LaTeX, HTML, CSS, Javascript, images, videos, sounds, and so on.

### Properties
1. Decoupled. The APIs for this should be written in such a way that bindings/implementations in other languages are possible, and mixing them is possible.
2. Portable. At minimum, a client/server implementation should be able to run on anything that could support an HTTP server. HTML/Javascript/CSS browsing support would be required if the user plans to utilize such features in their addons and cards.
3. Extensible. Various implementations of clients/servers could be written, and addons could similarly be written in a feature-level portable way. This would mean Javascript for clientside (which could cover anything the REST API touches) and Lua for serverside (extending the REST API). Serverside addons could probably be distributed in a portable way so long as there's a way to configure them to use native modules for multiple platforms. Portability of clientside addons would ultimately depend on the client being used. If they're natively written without a full HTML interface, addon authors would need to be able to check what platform/client their addon is running on and select an implementation accordingly, assuming the client supports it in some way. I think that clientside addons would be best wrapped in Lua with a common interface for loading/injection purposes at the very least.

In an ideal world someone could write a client implementation as a CLI or even in ncurses, and still utilize serverside addons that don't rely on HTML/JS, but clientside display of HTML with addons written in Javascript could not be supported. They could, of course, still rep. The clientside addons would still be synced and usable on supported clients. If the person who made the ncurses client was feeling particularly generous, they might implement their own clientside addon framework which could of course be synced. If the design of the server/model is particularly good, the UI would be described by a model that could be implemented on clients to facilitate maximum addon portability.

One hardmode use-case would be a media capture plugin. This is extremely platform dependent and is difficult to accomplish via browser alone. Many browsers will support [MediaStreamRecorder](https://github.com/streamproc/MediaStreamRecorder) and/or [RecordRTC](https://github.com/muaz-khan/RecordRTC), but for some platforms it may be necessary to implement it using some combination of Lua and native modules. It seems the best bet would be to use [Cordova](https://cordova.apache.org/) with [cordova-media-capture-plugin](https://github.com/apache/cordova-plugin-media-capture) for the quirky mobile platforms. A lot of the quirks in that plugin can be mitigated by the decoupled architecture of this application.

## Components

This loosely follows an MVC pattern.

- SRS Server - Self-hosted daemon that implements a RESTful API for managing collections. This would interact with the Collection Repository and facilitate syncing with the Collection Remotes. It would also provide SSH key management. This is the controller.
- SRS Client - The method by which the SRS Server is accessed. This could be via your browser with simple HTML, perhaps even generated using something like Jekyll. Perhaps the SRS Server could provide an HTML interface. A custom UI version (maybe backed by webkit for the lazy) is another option. In fact, an elaborate Anki addon could be used to turn Anki into a Kioku client. This is the view.
- Collection Repository - The local repository that contains decks and media. This is the model.
- Collection Hosts - Remotes to use for syncing.

The SRS Server interacts with the Collection Repository, which can sync to Collection Remotes. In fact, the SRS Server is completely decoupled in that you could connect to a remote one, sync to your repository host(s), and pick it up again on a local server. Some SRS Server implementations could serve extra HTML that offer other features like managing/syncing remotes via pull request.

Using these decoupled components, anyone could write an implementation of any component with the guarantee that it "just works" with arbitrary implementations of any other component. Here are some examples.

- Default case: a local server and local client. The user could hotswap clients and servers if they like. Local repository management and syncing is of course managed by the local server via the local client. They might tell it to sync with something similar to AnkiWeb and then using it via that interface would just automagically work.
- The default case could be replicated on a mobile device by simply creating a mobile webapp and have the server written specially for the device.
- Write both the client and server implementation and host it to get something like AnkiWeb. They could even abstract away user management with something like the [Gitlab API](https://docs.gitlab.com/ce/api/users.html). If the server is hosted in a docker container, addons could safely be synced and used via the remote web interface without admins needing to worry.

## API Layers

The lowest layer is the Plumbing layer.
- Filesystem - One that watches/reads/writes data.
- History - One that versions data via Git.
- Sync - One that syncs data via Git.
- Plugin - Interface for plugins.
- Addons - Interface for addons.

Above that is the SRS layer.
- Model - One that builds and modifies a data model using Filesystem & History modules.
- Stats - One that computes statistics based on the Model module.
- Schedule - One that computes/modifies scheduling using the Model module. Example: [Anki Implementation](https://github.com/dae/anki/blob/master/anki/sched.py)
- Options - One that manages options using the Model module.
- Control - The control interface for SRSing which uses with the Model module.
- Delivery - One that composes content from the Model module for the user.

Above that is the Server layer.
- SRS Interface - The low-level API functions using the whole SRS Layer.
- REST - Uses the SRS Interface Module to implement the REST protocol on an HTTP Server.

Above that is the Client layer.
- Model - The API for interfacing with the SRS data model.
- Sync - The API for syncing Git databases.
- Schedule - The API for scheduling.
- Statistic - The API for computing statistics.
- REST - The API for implementing whatever layers are needed for the REST Server.
