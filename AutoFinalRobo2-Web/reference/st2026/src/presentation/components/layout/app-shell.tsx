import { useResellerAccount } from "@/presentation/hooks/use-reseller-account";
import { useDesktopAttention } from "@/presentation/hooks/use-desktop-attention";
import { useDesktopNotices } from "@/presentation/hooks/use-desktop-notices";
import { useSectionBackShortcut } from "@/presentation/hooks/use-section-back-shortcut";
import { useSidebarPreferences } from "@/presentation/hooks/use-sidebar-preferences";
import { SidebarPreferencesModal } from "@/presentation/components/layout/sidebar-preferences-modal";
import { useEffect, useRef, useState } from "react";
import { PublicCatalogSearch } from "@/presentation/components/catalog/public-catalog-search";
import { Link, NavLink, Outlet, useLocation } from "react-router-dom";
import {
  Info,
  Archive,
  ArrowLeft,
  Bell,
  Heart,
  Boxes,
  Brain,
  BatteryCharging,
  ChartNoAxesCombined,
  ChevronDown,
  LayoutDashboard,
  PanelLeft,
  MessageCircle,
  Mail,
  ShoppingBag,
  ShoppingCart,
  Store,
  Package,
  Ticket,
  Repeat2,
  Settings,
  Siren,
  Scale,
  SlidersHorizontal,
  Users,
  Wrench,
} from "lucide-react";
import type { PermissionCode } from "@/domain/models";
import { SettingsModal } from "@/presentation/components/modals/settings-modal";
  import { CartModal } from "@/presentation/components/cart/cart-modal";
  import { Tooltip } from "@/presentation/components/ui/tooltip";
  import { UserDropdownMenu } from "@/presentation/components/auth/user-dropdown-menu";
import { useSession } from "@/presentation/hooks/use-session";
import { useCart } from "@/presentation/hooks/use-cart";
import { useNotificationPolicy } from "@/presentation/hooks/use-notification-policy";
import { usePersonalNotifications } from "@/presentation/hooks/use-personal-notifications";
import { InboxEntryLink, type InboxEntry } from "@/presentation/components/notifications/inbox-entry";
import type { NotificationCategory } from "@/shared/notifications/policy";
import { cn } from "@/shared/cn";
import { toast } from "sonner";
import { PublicSaleAlert } from "@/presentation/components/sales/public-sale-alert";
import { WhatsAppAttentionAlert } from "@/presentation/components/whatsapp/attention-alert";
import { PaymentReviewAlert } from "@/presentation/components/whatsapp/payment-review-alert";
import { WhatsAppAttentionProvider, useWhatsAppAttention } from "@/presentation/hooks/use-whatsapp-attention";
import { ContactUnreadProvider, useContactUnread } from "@/presentation/hooks/use-contact-unread";
import { useCloudUser } from "@/presentation/hooks/use-cloud-user";
import { firebaseApiHeaders } from "@/infrastructure/firebase/browser";
import { sosStaffApi } from "@/infrastructure/http/sos-staff-api";
import { preorderStaffApi } from "@/infrastructure/http/preorder-staff-api";
import type { PreorderNotification } from "@/shared/preorders/contracts";
import { BaseModal } from "@/presentation/components/modals/base-modal";
import { Button } from "@/presentation/components/ui/button";
import RadalHorizontal from "@/presentation/images/radal_horizontal.png";
import RadalHorizontalLight from "@/presentation/images/radal_horizontal_light.png";
import RadalMark from "@/presentation/images/radal_isologo.png";
import RadalMarkLight from "@/presentation/images/radal_isologo_light.png";


import { ThemeLogo } from "@/presentation/components/ui/theme-logo";
import { DesktopShortcutButtons } from "@/desktop/DesktopShortcuts";
import { desktopShortcutLabel } from "@/shared/desktop/shortcuts";
import { ExpandableBackLink } from "@/presentation/components/ui/expandable-back-link";

import { AdminNavigation, getAdminNavigation } from "@/presentation/components/admin/admin-navigation";
import { getModuleNavigation } from "@/presentation/components/layout/module-navigation";
import { WorkspaceNavigation } from "@/presentation/components/layout/workspace-navigation";

import { ResponsivePublicActions } from "@/presentation/components/ui/responsive-public-actions";
import "@/presentation/styles/app-shell-mobile.css";
import "@/presentation/styles/app-shell-sidebar.css";

const SIDEBAR_STORAGE_KEY = "radal.internal-sidebar-collapsed";

const navigation: Array<{
  to: string;
  label: string;
  icon: React.ComponentType<{ className?: string }>;
  permission: PermissionCode;
  desktopOnly?: boolean;
}> = [
  { to: ".", label: "Inicio", icon: LayoutDashboard, permission: "dashboard.ver" },
  { to: "clientes", label: "Clientes", icon: Users, permission: "clientes.ver" },
  { to: "catalogo", label: "Catálogo", icon: Boxes, permission: "catalogo.ver" },
  { to: "servicios", label: "Servicios", icon: Wrench, permission: "catalogo.editar", desktopOnly: true },
  { to: "tienda-online", label: "Tienda online", icon: Store, permission: "catalogo.ver" },
  { to: "stock", label: "Baterías", icon: BatteryCharging, permission: "stock.ver" },
  { to: "ventas", label: "Ventas e historial", icon: ChartNoAxesCombined, permission: "ventas.ver" },
  { to: "garantias", label: "Garantías y controles", icon: BatteryCharging, permission: "garantias.ver" },
  { to: "pedidos-online", label: "Pedidos online", icon: ShoppingBag, permission: "pedidos_online.ver" },
  { to: "atencion/whatsapp", label: "WhatsApp", icon: MessageCircle, permission: "whatsapp.ver", desktopOnly: true },
  { to: "whatsapp", label: "Pedidos WhatsApp", icon: MessageCircle, permission: "whatsapp.ver", desktopOnly: true },
  { to: "encargos", label: "Encargos de productos", icon: Package, permission: "encargos.ver" },
  { to:"cotizaciones",label:"Cotizaciones",icon:Ticket,permission:"encargos_especiales.ver" },
  { to: "encargos-especiales", label: "Encargos especiales", icon: Brain, permission: "encargos_especiales.ver" },
  { to: "suscripciones", label: "Suscripciones", icon: Repeat2, permission: "suscripciones.ver" },
  { to: "atencion/contacto", label: "Contacto", icon: Mail, permission: "dashboard.ver" },
  { to: "remitos", label: "Remitos", icon: Archive, permission: "remitos.ver" },
  { to: "administracion", label: "Administración", icon: SlidersHorizontal, permission: "administracion.ver" },
  { to: "revendedores", label: "Portal de revendedores", icon: Users, permission: "revendedores.ver" },
  { to: "metricas", label: "Métricas Plus", icon: ChartNoAxesCombined, permission: "metricas.ver" },
  { to: "reportes", label: "Reportes", icon: ChartNoAxesCombined, permission: "reportes.ver" },
  { to: "balances", label: "Balances", icon: Scale, permission: "balances.ver" },
];

const clientNavigation = [
  { to: "reventa", label: "Mis baterías para reventa", icon: BatteryCharging },
  { to: ".", label: "Inicio", icon: LayoutDashboard },
  { to: "catalogo", label: "Tienda online", icon: Store },
  { to: "carrito", label: "Carrito", icon: ShoppingCart },
  { to:"cotizaciones",label:"Cotizaciones",icon:Ticket,permission:"encargos_especiales.ver" },
  { to: "encargos-especiales", label: "Encargos especiales", icon: Brain },
  { to: "suscripciones", label: "Mis suscripciones", icon: Repeat2 },
];

export function AppShell({ desktop = false }: { desktop?: boolean }) {
  const policy = useNotificationPolicy(desktop);
  return <WhatsAppAttentionProvider policy={policy}><ContactUnreadProvider><AppShellContent desktop={desktop} policy={policy} /></ContactUnreadProvider></WhatsAppAttentionProvider>;
}

function AppShellContent({ desktop = false, policy }: { desktop?: boolean; policy: Record<NotificationCategory, boolean> }) {
  const { currentUser, session, logout, state } = useSession();
  const { totalItems: cartItemCount } = useCart();
  const sidebarPreferences = useSidebarPreferences(session?.userId, desktop);
  const [isSidebarPreferencesOpen, setIsSidebarPreferencesOpen] = useState(false);
  const resellerAccount=useResellerAccount();
  const location = useLocation();
  const adminNavigation = getAdminNavigation(location.pathname, desktop);
  const {user:cloudUser}=useCloudUser();
  const { client: whatsappClient, pendingAlerts, commercialAlerts, soundEnabled, enableSound, refresh: refreshWhatsApp } = useWhatsAppAttention();
  const contactUnread = useContactUnread();
  const personal = usePersonalNotifications(desktop);
  const [internalReadState, setInternalReadState] = useState<{ owner: string; ids: string[] }>({ owner: "", ids: [] });
  const internalRead = new Set(internalReadState.owner === session?.userId ? internalReadState.ids : []);
  const [preorderNotifications,setPreorderNotifications]=useState<PreorderNotification[]>([]);
  const [activePreorderNotification,setActivePreorderNotification]=useState<PreorderNotification|null>(null);
  const [accountNotifications,setAccountNotifications]=useState<Array<{uid:string;version:number;name:string;email:string}>>([]);
  const [accountNotificationCount,setAccountNotificationCount]=useState(0);
  const [sosNotificationCount,setSosNotificationCount]=useState(0);
  const seenAccountNotifications=useRef(new Set<string>());
  const desktopNotices=useDesktopNotices(desktop,session?.userId);
  const desktopAttention=useDesktopAttention(desktop,session,policy);
  const visibleWhatsApp = policy.WHATSAPP && session?.permissionCodes.includes("whatsapp.ver") ? pendingAlerts : [];
  const visibleCommercial = commercialAlerts.filter(alert => alert.type === "PAYMENT_REVIEW" || alert.type === "RECEIPT_FILE_FAILED" ? policy.PAYMENTS && session?.permissionCodes.includes("whatsapp.gestionar") : policy.OPERATIONS && session?.permissionCodes.includes("whatsapp.ver"));
  const visiblePreorders = policy.PREORDERS && session?.permissionCodes.includes("encargos.ver") ? preorderNotifications.filter(item => !item.read) : [];
  const visibleAccountCount = policy.ACCOUNT_LINKS && session?.permissionCodes.includes("administracion.ver") ? accountNotificationCount : 0;

  const whatsappAttentionPath = desktop ? "/atencion/whatsapp" : "/plataforma/atencion/whatsapp";
  const sosPath = desktop ? "/moura-sos" : "/plataforma/moura-sos";
  const isClient = Boolean(session?.roleCodes.some(role=>role==="cliente"||role==="cm") && !session.roleCodes.some(role => role === "vendedor" || role === "administrador" || role === "desarrollador"));
  const contactCounts = {
    whatsapp: contactUnread.whatsapp.size + visibleWhatsApp.filter(conversation => !contactUnread.whatsapp.has(conversation.conversationId)).length + visibleCommercial.length,
    mail: contactUnread.mail.size,
    reviewWhatsApp: contactUnread.whatsappReview.size > 0 && contactUnread.whatsapp.size === contactUnread.whatsappReview.size,
  };
  const moduleNavigation = !isClient ? getModuleNavigation(location, desktop, session?.permissionCodes ?? [], contactCounts) : null;
  const sidebarNavigation = adminNavigation ?? moduleNavigation;
  const sectionPreferences = useSidebarPreferences(session?.userId, desktop, sidebarNavigation?.title);
  const organizationPreferences = sidebarNavigation ? sectionPreferences : sidebarPreferences;
  const sectionEntries = sectionPreferences.arrange(sidebarNavigation?.items.map(item => ({ to: item.label, label: item.label })) ?? []);
  const consolidateCatalog = !isClient && Boolean(session?.permissionCodes.includes("catalogo.ver"));
  const allowedNavigation = isClient && !desktop
    ? [...clientNavigation.filter(item=>item.to!=="reventa"||Boolean(resellerAccount.profile)), ...(session?.permissionCodes.includes("metricas.ver") ? [{to:"metricas",label:"Métricas Plus",icon:ChartNoAxesCombined}] : [])]
    : [...navigation.filter((item) => (desktop && item.to === "reportes" ? session?.permissionCodes.some(code => code === "reportes.ver" || code === "balances.ver") : session?.permissionCodes.includes(item.permission)) && !(desktop && item.to === "balances") && !(desktop && item.to === "metricas") && (!item.desktopOnly || desktop) && !(consolidateCatalog && ["tienda-online", "stock", "servicios"].includes(item.to))).map(item => desktop && item.to === "reportes" ? {...item,label:"Balance y reportes"} : item)];
  const orderedNavigation = sidebarPreferences.arrange(allowedNavigation);
  const isFixedClientAccess = (to: string) => isClient && !desktop && to === "carrito";
  const visibleNavigation = orderedNavigation.filter(item => isFixedClientAccess(item.to) || sidebarPreferences.isVisible(item.to));
  const [isSettingsOpen, setIsSettingsOpen] = useState(false);
  const [isUserMenuOpen, setIsUserMenuOpen] = useState(false);
  const [isNotificationsOpen, setIsNotificationsOpen] = useState(false);
  const [isSidebarCollapsed, setIsSidebarCollapsed] = useState(() => {
    try {
      return window.localStorage.getItem(SIDEBAR_STORAGE_KEY) === "true";
    } catch {
      return false;
    }
  });
  const profileRef = useRef<HTMLDivElement>(null);
  const [isMobile, setIsMobile] = useState(() => window.matchMedia?.("(max-width: 1023px)").matches ?? window.innerWidth < 1024);
  const [isMobileSidebarOpen, setIsMobileSidebarOpen] = useState(false);
  const sidebarRef = useRef<HTMLElement>(null);
  const contentRef = useRef<HTMLDivElement>(null);
  const sidebarTriggerRef = useRef<HTMLButtonElement>(null);
  const sidebarCollapsed = !isMobile && isSidebarCollapsed;

  useSectionBackShortcut({
    home: desktop ? "/" : "/plataforma",
    owner: session?.mustChangePassword ? undefined : session?.userId,
    dismissOverlay: () => {
      if (isUserMenuOpen) { setIsUserMenuOpen(false); return true; }
      if (isNotificationsOpen) { setIsNotificationsOpen(false); return true; }
      if (isMobileSidebarOpen) { setIsMobileSidebarOpen(false); return true; }
      return false;
    },
  });

  useEffect(() => {
    const media = window.matchMedia?.("(max-width: 1023px)");
    if (!media) return;
    const update = () => {
      setIsMobile(media.matches);
      if (!media.matches) setIsMobileSidebarOpen(false);
    };
    media.addEventListener("change", update);
    return () => media.removeEventListener("change", update);
  }, []);

  useEffect(() => {
    setIsMobileSidebarOpen(false);
    setIsNotificationsOpen(false);
    setIsUserMenuOpen(false);
  }, [location.pathname, location.search, location.hash]);

  useEffect(() => {
    const sidebar = sidebarRef.current;
    const content = contentRef.current;
    if (sidebar) sidebar.inert = isMobile && !isMobileSidebarOpen;
    if (content) content.inert = isMobile && isMobileSidebarOpen;
    if (!isMobile || !isMobileSidebarOpen || !sidebar) return;
    setIsNotificationsOpen(false);
    setIsUserMenuOpen(false);
    const previousOverflow = document.body.style.overflow;
    document.body.style.overflow = "hidden";
    sidebar.focus({ preventScroll: true });
    function handleKeyboard(event: KeyboardEvent) {
      if (event.key === "Escape") {
        event.preventDefault();
        setIsMobileSidebarOpen(false);
      }
      if (event.key !== "Tab") return;
      const focusable = Array.from(sidebar!.querySelectorAll<HTMLElement>('a[href], button:not([disabled]), [tabindex="0"]'))
        .filter(element => element.getClientRects().length > 0);
      const first = focusable[0];
      const last = focusable[focusable.length - 1];
      if (event.shiftKey && (document.activeElement === first || document.activeElement === sidebar)) { event.preventDefault(); last?.focus(); }
      else if (!event.shiftKey && document.activeElement === last) { event.preventDefault(); first?.focus(); }
    }
    document.addEventListener("keydown", handleKeyboard);
    return () => {
      document.body.style.overflow = previousOverflow;
      document.removeEventListener("keydown", handleKeyboard);
      if (content) content.inert = false;
      sidebarTriggerRef.current?.focus();
    };
  }, [isMobile, isMobileSidebarOpen]);

  useEffect(() => {
    try {
      window.localStorage.setItem(SIDEBAR_STORAGE_KEY, String(isSidebarCollapsed));
    } catch {
      // El estado sigue funcionando aunque el navegador no permita persistirlo.
    }
  }, [isSidebarCollapsed]);

  useEffect(()=>{if(!policy.PREORDERS||!session?.permissionCodes.includes("encargos.ver")||(!desktop&&(!cloudUser||cloudUser.isAnonymous)))return;let active=true;const refresh=async()=>{try{const result=desktop?await window.desktop.preorders.notifications():await preorderStaffApi.notifications(cloudUser!);if(!active)return;setPreorderNotifications(result.items);setActivePreorderNotification(current=>current??result.items.find(item=>!item.read)??null);}catch{/* La página de encargos mostrará el error si el operador entra. */}};void refresh();const timer=window.setInterval(()=>void refresh(),30_000);return()=>{active=false;window.clearInterval(timer);};},[desktop,cloudUser?.uid,session?.permissionCodes.includes("encargos.ver"),policy.PREORDERS]);
  useEffect(() => {
    const owner = session?.userId ?? "";
    try {
      const saved = JSON.parse(window.localStorage.getItem("st2026.notification-reads." + owner) ?? "[]") as unknown;
      setInternalReadState({ owner, ids: Array.isArray(saved) ? saved.filter((id): id is string => typeof id === "string").slice(-500) : [] });
    } catch { setInternalReadState({ owner, ids: [] }); }
  }, [session?.userId]);
  function markInternalRead(id: string) {
    const owner = session?.userId;
    if (!owner) return;
    setInternalReadState(current => {
      const ids = [...new Set([...(current.owner === owner ? current.ids : []), id])].slice(-500);
      try { window.localStorage.setItem("st2026.notification-reads." + owner, JSON.stringify(ids)); } catch { /* El estado sigue vigente durante la sesión. */ }
      return { owner, ids };
    });
  }
  useEffect(()=>{seenAccountNotifications.current.clear();setAccountNotifications([]);setAccountNotificationCount(0);setPreorderNotifications([]);setActivePreorderNotification(null);setSosNotificationCount(0);setIsNotificationsOpen(false);},[session?.userId]);
  useEffect(()=>{if(!policy.PREORDERS){setPreorderNotifications([]);setActivePreorderNotification(null);}if(!policy.ACCOUNT_LINKS){setAccountNotifications([]);setAccountNotificationCount(0);}if(!policy.SOS)setSosNotificationCount(0);},[policy.PREORDERS,policy.ACCOUNT_LINKS,policy.SOS]);
  useEffect(()=>{if(!desktop||!policy.ACCOUNT_LINKS||!session?.permissionCodes.includes("administracion.ver"))return;let active=true;const refresh=async()=>{try{const result=await window.desktop.accounts.notifications();if(!active)return;setAccountNotifications(result.requests);setAccountNotificationCount(result.pendingCount);for(const request of result.requests){const key=`${request.uid}:${request.version}`;if(!seenAccountNotifications.current.has(key)){seenAccountNotifications.current.add(key);toast.info("Nueva solicitud de asociación",{description:`${request.name} · ${request.email}`});}}}catch{/* La bandeja mantiene su propio estado de conexión. */}};void refresh();const timer=window.setInterval(()=>void refresh(),30_000);return()=>{active=false;window.clearInterval(timer);};},[desktop,session?.userId,session?.permissionCodes.includes("administracion.ver"),policy.ACCOUNT_LINKS]);
  useEffect(()=>{if(!policy.SOS||!cloudUser||cloudUser.isAnonymous||!session?.permissionCodes.includes("moura_sos.ver")){setSosNotificationCount(0);return;}let active=true;const refresh=async()=>{try{const result=await sosStaffApi.list(cloudUser);if(active)setSosNotificationCount(result.items.filter(item=>!["COMPLETED","CANCELLED"].includes(item.serviceStatus)||["REJECTED","CHARGED_BACK","ERROR"].includes(item.paymentStatus)).length);}catch{/* La pantalla SOS mantiene su propio estado de error. */}};void refresh();const timer=window.setInterval(()=>void refresh(),30_000);return()=>{active=false;window.clearInterval(timer);};},[cloudUser?.uid,session?.permissionCodes.includes("moura_sos.ver"),policy.SOS]);
  useEffect(()=>{if(desktop||!policy.ACCOUNT_LINKS||!cloudUser||cloudUser.isAnonymous||!session?.permissionCodes.includes("administracion.ver"))return;let active=true;const refresh=async()=>{try{const response=await fetch("/api/v1/staff/account-requests",{cache:"no-store",headers:await firebaseApiHeaders(cloudUser)});if(!response.ok)return;const body=await response.json() as {requests?:unknown[]};if(active)setAccountNotificationCount(Array.isArray(body.requests)?body.requests.length:0);}catch{/* La bandeja de Administración conserva el error detallado. */}};void refresh();const timer=window.setInterval(()=>void refresh(),30_000);return()=>{active=false;window.clearInterval(timer);};},[desktop,cloudUser?.uid,session?.permissionCodes.includes("administracion.ver"),policy.ACCOUNT_LINKS]);
  useEffect(()=>{if(!policy.PREORDERS||!activePreorderNotification||!soundEnabled)return;try{const context=new AudioContext(),oscillator=context.createOscillator(),gain=context.createGain();oscillator.frequency.value=activePreorderNotification.kind==="DEPOSIT_APPROVED"?880:660;gain.gain.setValueAtTime(.06,context.currentTime);gain.gain.exponentialRampToValueAtTime(.001,context.currentTime+.22);oscillator.connect(gain).connect(context.destination);oscillator.start();oscillator.stop(context.currentTime+.22);oscillator.onended=()=>void context.close();}catch{/* El navegador puede bloquear audio hasta que el operador lo habilite. */}},[activePreorderNotification?.notificationId,soundEnabled,policy.PREORDERS]);
  async function acknowledgePreorder(notification:PreorderNotification){if(desktop)await window.desktop.preorders.readNotification({notificationId:notification.notificationId});else if(cloudUser)await preorderStaffApi.readNotification(cloudUser,notification.notificationId);else return;setPreorderNotifications(items=>items.map(item=>item.notificationId===notification.notificationId?{...item,read:true}:item));setActivePreorderNotification(null);}

  const closeNotifications = () => setIsNotificationsOpen(false);
  const unmatchedWhatsApp=[...contactUnread.whatsapp].filter(id=>!visibleWhatsApp.some(item=>item.conversationId===id));
  const entries: InboxEntry[] = [
    ...(policy.WHATSAPP&&unmatchedWhatsApp.length?[{id:"contact:whatsapp:"+unmatchedWhatsApp.sort().join(","),title:"Conversaciones sin leer",description:`${unmatchedWhatsApp.length} conversación(es) de WhatsApp`,href:desktop?"/atencion/contacto?canal=whatsapp":"/plataforma/atencion/contacto?canal=whatsapp"+(unmatchedWhatsApp.every(id=>contactUnread.whatsappReview.has(id))?"&prueba=meta":""),module:"atencion/contacto",priority:1,category:"WHATSAPP",read:false,task:true,onOpen:closeNotifications}]:[]),
    ...(policy.OPERATIONS && contactUnread.mail.size ? [{id:"contact:mail:"+[...contactUnread.mail].sort().join(","),title:"Correo sin leer",description:`${contactUnread.mail.size} mensaje(s) en la bandeja de entrada`,href:desktop?"/atencion/contacto?canal=correo":"/plataforma/atencion/contacto?canal=correo",module:"atencion/contacto",priority:1,category:"OPERATIONS",read:false,task:true,onOpen:closeNotifications}]:[]),
    ...desktopAttention.map(item=>({...item,onOpen:()=>{closeNotifications();markInternalRead(item.id);}})),
    ...desktopNotices.items.filter(()=>policy.OPERATIONS).map(item=>({...item,category:"OPERATIONS",onOpen:()=>{closeNotifications();void desktopNotices.read(item.id).catch(()=>toast.error("No se pudo marcar el aviso."));}})),
    ...personal.items.map(item => ({
      id: "personal:" + item.id, title: item.title, description: item.description, href: item.destination,
      category: item.category, createdAt: item.createdAt, read: item.read,
      onOpen: () => { closeNotifications(); void personal.markRead(item.id).catch(() => toast.error("No se pudo marcar el aviso como leído.")); },
    })),
    ...(visibleWhatsApp.length ? [{
      id: "staff:whatsapp:" + visibleWhatsApp.map(item => item.conversationId).sort().join(","),
      title: "Atención por WhatsApp",
      description: visibleWhatsApp.length + " conversación" + (visibleWhatsApp.length === 1 ? "" : "es") + " pendiente" + (visibleWhatsApp.length === 1 ? "" : "s"),
      href: desktop?"/atencion/contacto?canal=whatsapp":whatsappAttentionPath, category: "WHATSAPP", read: false, task:true,
      onOpen: () => { closeNotifications(); markInternalRead("staff:whatsapp:" + visibleWhatsApp.map(item => item.conversationId).sort().join(",")); },
    }] : []),
    ...visibleCommercial.map(alert => ({
      id: "staff:commercial:" + alert.alertId, title: alert.type === "RESELLER_SALE_REVIEW" ? "Venta externa para revisar" : alert.type === "PAYMENT_REVIEW" ? "Pago para revisar" : alert.type === "RECEIPT_FILE_FAILED" ? "Comprobante para recuperar" : alert.type === "RECOVERY_FAILED" ? "Alerta de recuperación" : "Presupuesto enviado",
      description: (alert.name ?? alert.whatsappPhone) + " · " + (alert.quoteId ?? "sin presupuesto asociado"),
      href: alert.type==="RESELLER_SALE_REVIEW"?(desktop?"/revendedores":"/plataforma/revendedores")+(alert.externalSaleReportId?"?reporte="+encodeURIComponent(alert.externalSaleReportId):""):whatsappAttentionPath + "?conversacion=" + encodeURIComponent(alert.conversationId), category: alert.type==="RESELLER_SALE_REVIEW"?"OPERATIONS":"PAYMENTS", read: false, task:true,
      onOpen: () => { closeNotifications(); markInternalRead("staff:commercial:" + alert.alertId); if (whatsappClient && session?.permissionCodes.includes("whatsapp.gestionar")) void whatsappClient.acknowledgeCommercialAlert(alert.alertId).then(refreshWhatsApp).catch(() => undefined); },
    })),
    ...visiblePreorders.map(item => ({
      id: "staff:preorder:" + item.notificationId, title: item.kind === "DEPOSIT_APPROVED" ? "Seña acreditada" : "Nuevo encargo",
      description: item.preorderNumber + " · " + item.customerName,
      href: desktop ? "/encargos" : "/plataforma/encargos", category: "PREORDERS", read: item.read,
      onOpen: () => { closeNotifications(); markInternalRead("staff:preorder:" + item.notificationId); void acknowledgePreorder(item).catch(() => toast.error("No se pudo marcar el encargo como leído.")); },
    })),
    ...(visibleAccountCount ? [{
      id: "staff:accounts:" + visibleAccountCount, title: "Solicitudes de asociación",
      description: visibleAccountCount + " cuenta" + (visibleAccountCount === 1 ? "" : "s") + " pendiente" + (visibleAccountCount === 1 ? "" : "s"),
      href: desktop ? "/administracion" : "/plataforma/administracion", category: "ACCOUNT_LINKS", read: false, task:true,
      onOpen: () => { closeNotifications(); markInternalRead("staff:accounts:" + visibleAccountCount); },
    }] : []),
    ...(policy.SOS && sosNotificationCount ? [{
      id: "staff:sos:" + sosNotificationCount, title: "Auxilios SOS",
      description: sosNotificationCount + " solicitud" + (sosNotificationCount === 1 ? "" : "es") + " para revisar",
      href: sosPath, category: "SOS", read: false, task:true,
      onOpen: () => { closeNotifications(); markInternalRead("staff:sos:" + sosNotificationCount); },
    }] : []),
  ].map(entry => ({ ...entry, read: entry.read || internalRead.has(entry.id) }));
  function badgeCount(to:string){if(isFixedClientAccess(to))return cartItemCount>99?"99+":cartItemCount;const paths=desktop?`/${to}`:`/plataforma/${to}`;if(to==="atencion/whatsapp"){const count=contactCounts.whatsapp;return count>99?"99+":count;}if(to==="atencion/contacto"){const count=contactCounts.mail+contactCounts.whatsapp;return count>99?"99+":count;}if(to==="encargos")return visiblePreorders.length>99?"99+":visiblePreorders.length;if(to==="administracion")return visibleAccountCount+desktopAttention.filter(i=>i.module===to).length>99?"99+":visibleAccountCount+desktopAttention.filter(i=>i.module===to).length;const related=entries.filter(e=>(e.module===to||e.href.split("?")[0]===paths||(to==="atencion/contacto"&&e.href.includes("/atencion/whatsapp")))&&(e.task||!e.read));const count=related.length;return count>99?"99+":count;}
  const notificationCount = entries.filter(entry => !entry.read).length;
  const notificationPath = desktop ? "/notificaciones" : "/plataforma/notificaciones";
  const refreshNotifications = async () => { await Promise.all([personal.refresh(), refreshWhatsApp(), desktopNotices.refresh(), contactUnread.refresh()]); };


  useEffect(() => {
    if (!isUserMenuOpen) return;

    function closeProfileMenu(event: PointerEvent) {
      if (!profileRef.current?.contains(event.target as Node)) setIsUserMenuOpen(false);
    }
    function closeOnEscape(event: KeyboardEvent) {
      if (event.key === "Escape") setIsUserMenuOpen(false);
    }

    document.addEventListener("pointerdown", closeProfileMenu);
    document.addEventListener("keydown", closeOnEscape);
    return () => {
      document.removeEventListener("pointerdown", closeProfileMenu);
      document.removeEventListener("keydown", closeOnEscape);
    };
  }, [isUserMenuOpen]);

  function renderNavigationItem(item: (typeof visibleNavigation)[number]) {
    const shortcut = desktop ? desktopShortcutLabel(item.to === "." ? "/" : `/${item.to}`) : undefined;
    const base = desktop ? "" : "/plataforma";
    const destinationPath = item.to === "." ? (base || "/") : `${base}/${item.to}`;
    const destination = item.to === "atencion/contacto" ? `${destinationPath}${contactUnread.whatsapp.size ? `?canal=whatsapp${contactCounts.reviewWhatsApp ? "&prueba=meta" : ""}` : contactUnread.mail.size ? "?canal=correo" : ""}` : destinationPath;
    const pathname = location.pathname.replace(/\/$/, "") || "/";
    const sectionActive = item.to === "."
      ? pathname === destinationPath
      : pathname === destinationPath || pathname.startsWith(destinationPath + "/") || (item.to === "catalogo" && pathname.startsWith(`${base}/tienda-online/`)) || (item.to === "ventas" && (pathname === `${base}/historial` || pathname.startsWith(`${base}/historial/`)));
    const link = (
      <NavLink end={item.to === "."} className={() => cn("app-shell__nav-item group relative flex w-full items-center text-sm", sidebarCollapsed ? "justify-center" : "gap-3", sectionActive ? "is-active" : "text-secondary")} key={item.to} aria-label={isFixedClientAccess(item.to) ? `Carrito, ${cartItemCount} ${cartItemCount === 1 ? "producto" : "productos"}` : undefined} aria-keyshortcuts={shortcut} title={shortcut ? `${item.label} · ${shortcut}` : item.label} aria-current={sectionActive ? "page" : undefined} data-section-active={sectionActive} to={destination}>
        <item.icon className="h-4 w-4 shrink-0" />
        <span className={cn("min-w-0 overflow-hidden",item.to === "revendedores" ? "whitespace-normal leading-snug" : "whitespace-nowrap",sidebarCollapsed?"w-0 opacity-0":"w-auto opacity-100")}>{item.label}</span>
        {badgeCount(item.to)?<span className={cn("ml-auto rounded-full bg-danger px-2 py-0.5 text-[10px] font-bold text-onStrong",sidebarCollapsed&&"absolute right-0 top-0 scale-75")}>{badgeCount(item.to)}</span>:null}
      </NavLink>
    );
    return shortcut || sidebarCollapsed ? <Tooltip className="w-full" content={shortcut ? `${item.label} · ${shortcut}` : item.label} key={item.to} side="right">{link}</Tooltip> : link;
  }

  function openSidebarPreferences() {
    setIsMobileSidebarOpen(false);
    setIsSidebarPreferencesOpen(true);
  }

  return (
    <div className={cn("app-shell flex min-h-screen bg-surface", sidebarCollapsed && "app-shell--sidebar-collapsed", isMobileSidebarOpen && "app-shell--navigation-open")}>
      <button aria-label="Cerrar navegación" aria-hidden={!isMobileSidebarOpen} className={cn("app-shell__scrim", isMobileSidebarOpen && "is-open")} onClick={() => setIsMobileSidebarOpen(false)} tabIndex={-1} type="button" />
      <aside className={cn("app-shell__sidebar fixed inset-y-0 left-0 z-30 hidden shrink-0 overflow-y-auto border-r border-subtle bg-elevated text-ink transition-[width,padding] duration-[240ms] ease-[var(--ease-drawer)] motion-reduce:transition-none lg:flex lg:flex-col", sidebarCollapsed ? "w-20" : "w-72", isMobileSidebarOpen && "is-open")} id="internal-sidebar" tabIndex={-1} ref={sidebarRef} role={isMobile ? "dialog" : undefined} aria-modal={isMobileSidebarOpen || undefined} aria-label="Navegación de Plataforma online" aria-hidden={isMobile && !isMobileSidebarOpen} onClick={event => { if (isMobile && (event.target as HTMLElement).closest("a")) setIsMobileSidebarOpen(false); }}>
        <div className={cn("app-shell__sidebar-heading", sidebarCollapsed && "is-collapsed")}>
          <div className={cn("app-shell__sidebar-brand", sidebarCollapsed && "is-collapsed")}>
          <Link aria-label="Ir al inicio de Radal" className="app-shell__sidebar-logo" to="/">
            <ThemeLogo src={sidebarCollapsed ? RadalMark : RadalHorizontal} lightSrc={sidebarCollapsed ? RadalMarkLight : RadalHorizontalLight} alt="Radal" />
          </Link>
          <Tooltip content={isMobile ? "Cerrar barra lateral" : sidebarCollapsed ? "Mostrar barra lateral" : "Colapsar barra lateral"} side="right">
            <button aria-controls="internal-sidebar" aria-expanded={isMobile ? isMobileSidebarOpen : !sidebarCollapsed} aria-label={isMobile ? "Cerrar barra lateral" : sidebarCollapsed ? "Mostrar barra lateral" : "Colapsar barra lateral"} className="app-shell__sidebar-collapse" onClick={() => isMobile ? setIsMobileSidebarOpen(false) : setIsSidebarCollapsed((collapsed) => !collapsed)} type="button">
              {sidebarCollapsed ? <PanelLeft aria-hidden="true" className="h-5 w-5" /> : <ArrowLeft aria-hidden="true" className="h-5 w-5" />}
            </button>
          </Tooltip>
          </div>
          {!sidebarCollapsed ? <div className="app-shell__sidebar-context"><span>{isClient ? "PORTAL DE CLIENTES" : desktop ? "PLATAFORMA LOCAL" : "PLATAFORMA ONLINE"}</span><h2>{sidebarNavigation?.title ?? (isClient ? "Mi cuenta" : "Menú principal")}</h2></div> : null}
        </div>
        {sidebarNavigation ? <div className="flex-1">{adminNavigation ? <AdminNavigation collapsed={sidebarCollapsed} desktop={desktop} /> : moduleNavigation ? <WorkspaceNavigation navigation={moduleNavigation} collapsed={sidebarCollapsed} desktop={desktop} /> : null}</div> : <nav aria-label="Navegación principal" className={cn("app-shell__navigation flex flex-1 flex-col", sidebarCollapsed && "items-center")}>
          {visibleNavigation.map(renderNavigationItem)}
        </nav>}
        <Tooltip className="mt-4 w-full border-t border-subtle" content="Organizar barra lateral" side="right"><button type="button" aria-label="Organizar barra lateral" onClick={openSidebarPreferences} className={cn("app-shell__nav-item flex w-full items-center gap-3 text-sm text-secondary", sidebarCollapsed && "justify-center")}><PanelLeft className="h-4 w-4 shrink-0" />{!sidebarCollapsed ? <span>Organizar barra lateral</span> : null}</button></Tooltip>
      </aside>

      <div ref={contentRef} className={cn("app-shell__content flex min-w-0 flex-1 flex-col transition-[padding-left] duration-[240ms] ease-[var(--ease-drawer)] motion-reduce:transition-none", isSidebarCollapsed ? "lg:pl-20" : "lg:pl-72")}>
        <header className={cn("app-shell__header sticky top-0 z-20 flex items-center gap-3 border-b border-subtle bg-elevated px-4 py-4 lg:px-8", desktop ? "justify-end" : "justify-between")}>
          <div className="app-shell__leading">
            <button aria-controls="internal-sidebar" aria-expanded={isMobileSidebarOpen} aria-label="Abrir barra lateral" className="app-shell__navigation-trigger flex h-11 w-11 shrink-0 items-center justify-center rounded-full border border-subtle bg-elevated text-ink shadow-control" onClick={() => setIsMobileSidebarOpen(true)} ref={sidebarTriggerRef} type="button">
              <PanelLeft aria-hidden="true" className="h-5 w-5" />
            </button>
          {!desktop ? (
            <ExpandableBackLink className="app-shell__brands-back app-shell__brands-back--desktop" direction="left" label="Volver a marcas" to="/marcas" />
          ) : null}
          </div>
          <div className="app-shell__controls flex items-center gap-3">
            {session && !session.mustChangePassword && (!desktop || session.permissionCodes.includes("catalogo.ver")) ? <PublicCatalogSearch desktop={desktop} key={session.userId} platform /> : null}
            {desktop ? <DesktopShortcutButtons /> : null}
{policy.SOS && session?.permissionCodes.includes("moura_sos.ver") ? <Tooltip content="Auxilios Moura SOS y Yuasa SOS"><Link aria-label="Auxilios Moura SOS y Yuasa SOS" className="relative flex h-11 w-11 items-center justify-center rounded-full border border-subtle bg-elevated text-secondary shadow-sm [transition-property:background-color,color,transform] duration-150 hover:bg-muted hover:text-ink active:scale-95" to={sosPath}><Siren className="h-5 w-5"/>{sosNotificationCount ? <span className="absolute right-1.5 top-1.5 flex h-4 min-w-4 items-center justify-center rounded-full bg-danger px-1 text-[10px] font-bold text-onStrong">{sosNotificationCount}</span> : null}</Link></Tooltip> : null}
{!desktop ? <ExpandableBackLink className="app-shell__brands-back app-shell__brands-back--mobile" direction="left" label="Volver a marcas" to="/marcas" /> : null}
{<div className="relative">
              <Tooltip content={desktop ? "Notificaciones · Alt+N" : "Notificaciones"} disabled={isNotificationsOpen}>
                <button
                  aria-label="Notificaciones"
                  className="relative flex h-11 w-11 items-center justify-center rounded-full border border-subtle bg-elevated text-secondary shadow-sm [transition-property:background-color,color,transform] duration-150 hover:bg-muted hover:text-ink active:scale-95"
                  onClick={() => setIsNotificationsOpen(!isNotificationsOpen)}
                  type="button"
                >
                  <Bell className="h-5 w-5" />
                  {notificationCount ? <span className="absolute right-1.5 top-1.5 flex h-4 min-w-4 items-center justify-center rounded-full bg-danger px-1 text-[10px] font-bold text-onStrong">{notificationCount}</span> : null}
                </button>
              </Tooltip>
              {isNotificationsOpen && (
                <div className="app-shell__notifications absolute right-0 mt-2 w-72 rounded-xl border border-subtle bg-elevated p-2 shadow-xl z-50">
                  <div className="px-3 py-2 border-b border-subtle">
                    <p className="text-sm font-medium">Notificaciones</p>
                  </div>
                  <div className="flex max-h-80 flex-col gap-1 overflow-y-auto py-2">
                    {entries.map(entry => <InboxEntryLink compact entry={entry} key={entry.id} />)}
                    {!soundEnabled && (policy.WHATSAPP || policy.PREORDERS) ? <button className="rounded-md px-3 py-2 text-left text-sm hover:bg-muted" onClick={() => void enableSound()} type="button"><p className="font-medium text-ink">Activar sonido</p><p className="text-xs text-secondary">Requiere una interacción del navegador.</p></button> : null}
                    {entries.length === 0 ? <p className="px-3 py-3 text-sm text-secondary">No tenés notificaciones</p> : null}
                  </div>
                  <div className="border-t border-subtle px-1 pt-2"><Link className="block rounded-[var(--radius-control)] px-3 py-2 text-sm font-semibold text-[var(--action-primary)] hover:bg-muted focus-visible:outline focus-visible:outline-2 focus-visible:outline-[var(--focus-ring)]" onClick={closeNotifications} to={notificationPath}>Administrar notificaciones</Link></div>
                </div>
              )}
            </div>}
            {!desktop ? <Tooltip content="Mis favoritos"><Link aria-label="Mis favoritos" aria-current={location.pathname === "/plataforma/favoritos" ? "page" : undefined} className="relative flex h-11 w-11 shrink-0 items-center justify-center rounded-full border border-subtle bg-elevated text-secondary shadow-sm [transition-property:background-color,color,transform] duration-150 hover:bg-muted hover:text-ink active:scale-95 focus-visible:outline focus-visible:outline-2 focus-visible:outline-[var(--focus-ring)]" onClick={() => setIsNotificationsOpen(false)} to="/plataforma/favoritos"><Heart aria-hidden="true" className="h-5 w-5" fill={location.pathname === "/plataforma/favoritos" ? "currentColor" : "none"} /></Link></Tooltip> : null}
            <div className="app-shell__secondary"><ResponsivePublicActions>
            <Tooltip content="Configuración">
              <button
                aria-label="Configuración"
                className="relative flex h-11 w-11 items-center justify-center rounded-full border border-subtle bg-elevated text-secondary shadow-sm [transition-property:background-color,color,transform] duration-150 hover:bg-muted hover:text-ink active:scale-95"
                onClick={() => setIsSettingsOpen(true)}
                type="button"
              >
                <Settings className="h-5 w-5" /><span className="app-shell__utility-label">Configuración</span>
              </button>
            </Tooltip>
            </ResponsivePublicActions></div>
              <div className="app-shell__profile relative" ref={profileRef as any}>
                <Tooltip content={currentUser?.fullName ?? "Perfil"} disabled={isUserMenuOpen}>
                  <button
                    aria-expanded={isUserMenuOpen}
                    aria-haspopup="menu"
                    aria-label={`Perfil de ${currentUser?.fullName ?? "usuario"}`}
                    className="relative flex h-11 w-11 shrink-0 items-center justify-center rounded-full border border-subtle bg-elevated p-0 text-ink shadow-sm [transition-property:background-color,border-color,box-shadow,transform] duration-150 ease-out hover:border-[var(--border-strong)] hover:bg-muted active:scale-95"
                    onClick={() => setIsUserMenuOpen((open) => !open)}
                    type="button"
                  >
                    <span className="flex h-8 w-8 shrink-0 items-center justify-center rounded-full bg-primary text-xs font-semibold text-onStrong">
                      {currentUser?.fullName.charAt(0)}
                    </span>
                    <ChevronDown
                      aria-hidden="true"
                      className={cn(
                        "absolute -bottom-0.5 -right-0.5 h-3.5 w-3.5 rounded-full bg-elevated p-0.5 text-secondary shadow-sm transition-transform duration-150",
                        isUserMenuOpen && "rotate-180",
                      )}
                    />
                  </button>
                </Tooltip>
              {isUserMenuOpen && (
                <UserDropdownMenu onClose={() => setIsUserMenuOpen(false)} desktop={desktop} />
              )}
            </div>

          </div>
          </header>

        <main className="min-w-0 flex-1 overflow-x-auto p-4 lg:p-8">
          {!desktop && state === "ONBOARDING_REQUIRED" && location.pathname !== "/plataforma/perfil" ? (
            <div className="mb-4 flex flex-wrap items-center justify-between gap-3 rounded-[var(--radius-card)] border border-subtle bg-elevated p-4 text-sm">
              <p className="text-secondary">Podés completar los datos de tu cuenta desde Mi perfil.</p>
              <Link className="font-semibold text-primary underline underline-offset-4" to="/plataforma/perfil">Completar mi perfil</Link>
            </div>
          ) : null}
          <Outlet context={{ entries, error: personal.error || desktopNotices.error, refresh: refreshNotifications }} />
        </main>
      </div>
      <BaseModal isOpen={policy.PREORDERS && Boolean(activePreorderNotification)} title={activePreorderNotification?.kind==="DEPOSIT_APPROVED"?"Seña acreditada":"Nuevo encargo recibido"} onClose={()=>{if(activePreorderNotification)void acknowledgePreorder(activePreorderNotification);}}>{activePreorderNotification?<div className="space-y-4"><div className={`rounded-[var(--radius-control)] p-4 ${activePreorderNotification.kind==="DEPOSIT_APPROVED"?"bg-[var(--feedback-success-soft)]":"bg-muted"}`}><p className="font-semibold">{activePreorderNotification.preorderNumber} · {activePreorderNotification.productName}</p><p className="mt-1 text-sm text-secondary">Cliente: {activePreorderNotification.customerName}</p>{activePreorderNotification.amountCents!==null?<p className="mt-2 text-lg font-bold">{new Intl.NumberFormat("es-AR",{style:"currency",currency:"ARS"}).format(activePreorderNotification.amountCents/100)}</p>:null}</div><div className="flex justify-end gap-2"><Button variant="secondary" onClick={()=>void acknowledgePreorder(activePreorderNotification)}>Reconocer</Button><Link to={desktop?"/encargos":"/plataforma/encargos"} onClick={()=>void acknowledgePreorder(activePreorderNotification)}><Button variant={activePreorderNotification.kind==="DEPOSIT_APPROVED"?"success":"info"}>Abrir encargos</Button></Link></div></div>:null}</BaseModal>
      <SettingsModal onClose={() => setIsSettingsOpen(false)} open={isSettingsOpen} />
      <SidebarPreferencesModal key={sidebarNavigation?.title ?? "main"} section={sidebarNavigation?.title} open={isSidebarPreferencesOpen} onClose={() => setIsSidebarPreferencesOpen(false)} items={sidebarNavigation ? sectionEntries : orderedNavigation.filter(item=>!isFixedClientAccess(item.to))} isVisible={organizationPreferences.isVisible} setVisible={organizationPreferences.setVisible} reorder={organizationPreferences.reorder} reset={organizationPreferences.reset} />
      {policy.OPERATIONS && session?.permissionCodes.includes("pedidos_online.ver") ? <PublicSaleAlert /> : null}
      {!desktop && policy.WHATSAPP && session?.permissionCodes.includes("whatsapp.ver") ? <WhatsAppAttentionAlert /> : null}
      {!desktop && policy.PAYMENTS && session?.permissionCodes.includes("whatsapp.gestionar") ? <PaymentReviewAlert /> : null}
    </div>
  );
}
